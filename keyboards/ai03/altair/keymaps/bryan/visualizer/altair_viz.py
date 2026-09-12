#!/usr/bin/env python3
"""Live keymap visualizer for the ai03 Altair (bryan keymap) on Linux/Wayland.

Runs as a single GTK4 application. The first instance becomes the daemon: it
keeps a raw HID connection to the keyboard open and tracks the active layer.
Every later launch just toggles the window of the running instance, which is
what the Hyprland keybind does.

    altair_viz.py --daemon    start hidden (autostart)
    altair_viz.py             show/hide the window (toggle)
    altair_viz.py --render out.png [--layer N]   offline render, no display needed

Protocol (see keymap.c): keyboard sends [0x01, layer] on each layer change and
in response to a [0x01] request.
"""

import argparse
import glob
import os
import re
import sys
import threading
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
KEYMAP_C = HERE.parent / "keymap.c"
KEYBOARD_JSON = HERE.parent.parent.parent / "keyboard.json"

USB_VID, USB_PID = 0xA103, 0x0022
RAW_USAGE_PAGE = bytes([0x06, 0x60, 0xFF])  # Usage Page (0xFF60)
MSG_LAYER = 0x01
APP_ID = "dev.bryan.AltairViz"

UNIT = 58          # pixels per key unit
PAD = 24           # window padding
KEY_GAP = 5

# --------------------------------------------------------------------------
# keymap.c parsing
# --------------------------------------------------------------------------

KC_LABELS = {
    "KC_NO": "", "XXXXXXX": "", "KC_TRNS": "▽", "_______": "▽",
    "KC_ESC": "Esc", "KC_TAB": "Tab", "KC_ENT": "Enter", "KC_SPC": "Space",
    "KC_BSPC": "Bksp", "KC_DEL": "Del", "KC_INS": "Ins",
    "KC_LSFT": "Shift", "KC_RSFT": "Shift", "KC_LCTL": "Ctrl", "KC_RCTL": "Ctrl",
    "KC_LALT": "Alt", "KC_RALT": "AltGr", "KC_LGUI": "Super", "KC_RGUI": "Super",
    "KC_MINS": "-", "KC_EQL": "=", "KC_LBRC": "[", "KC_RBRC": "]", "KC_BSLS": "\\",
    "KC_SCLN": ";", "KC_QUOT": "'", "KC_GRV": "`", "KC_COMM": ",", "KC_DOT": ".",
    "KC_SLSH": "/", "KC_UNDS": "_", "KC_PLUS": "+", "KC_LCBR": "{", "KC_RCBR": "}",
    "KC_LPRN": "(", "KC_RPRN": ")", "KC_EXLM": "!", "KC_AT": "@", "KC_HASH": "#",
    "KC_DLR": "$", "KC_PERC": "%", "KC_CIRC": "^", "KC_AMPR": "&", "KC_ASTR": "*",
    "KC_TILD": "~", "KC_PIPE": "|", "KC_COLN": ":", "KC_DQUO": '"', "KC_LT": "<",
    "KC_GT": ">", "KC_QUES": "?",
    "KC_LEFT": "←", "KC_DOWN": "↓", "KC_UP": "↑", "KC_RGHT": "→",
    "KC_HOME": "Home", "KC_END": "End", "KC_PGUP": "PgUp", "KC_PGDN": "PgDn",
    "KC_VOLU": "Vol+", "KC_VOLD": "Vol-", "KC_MUTE": "Mute",
    "KC_MPRV": "⏮", "KC_MPLY": "⏯", "KC_MNXT": "⏭",
    "KC_WBAK": "Back", "KC_WFWD": "Fwd",
    "QK_BOOT": "BOOT", "AP_GLOB": "🌐",
    "KC_MISS": "Mission", "KC_LAUNCH": "Launch",
}
MOD_LABELS = {
    "LCTL": "Ctrl", "RCTL": "Ctrl", "LSFT": "Shift", "RSFT": "Shift",
    "LALT": "Alt", "RALT": "AltGr", "LGUI": "Super", "RGUI": "Super",
    "C": "Ctrl", "S": "Shift", "A": "Alt", "G": "Super",
}
OSM_LABELS = {
    "MOD_LCTL": "Ctrl", "MOD_RCTL": "Ctrl", "MOD_LSFT": "Shift", "MOD_RSFT": "Shift",
    "MOD_LALT": "Alt", "MOD_RALT": "AltGr", "MOD_LGUI": "Super", "MOD_RGUI": "Super",
}


def split_top_level(s):
    """Split on commas not nested inside parentheses."""
    out, depth, cur = [], 0, []
    for ch in s:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            out.append("".join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    tail = "".join(cur).strip()
    if tail:
        out.append(tail)
    return out


class Keymap:
    def __init__(self, path=KEYMAP_C):
        src = path.read_text()
        src = re.sub(r"//.*", "", src)
        src = re.sub(r"/\*.*?\*/", "", src, flags=re.S)
        self.defines = dict(re.findall(r"^#define\s+(\w+)\s+(.+?)\s*$", src, re.M))
        m = re.search(r"enum\s+layers\s*\{(.*?)\}", src, re.S)
        self.layer_names = [n.strip().lstrip("_") for n in m.group(1).split(",") if n.strip()]
        self.layer_index = {"_" + n: i for i, n in enumerate(self.layer_names)}
        self.layers = []
        for m in re.finditer(r"\[\s*\w+\s*\]\s*=\s*LAYOUT\s*\(", src):
            start = m.end()
            depth, i = 1, start
            while depth:
                depth += {"(": 1, ")": -1}.get(src[i], 0)
                i += 1
            self.layers.append(split_top_level(src[start:i - 1]))

    # -- keycode -> (main label, hold label) ---------------------------------
    def resolve(self, kc):
        kc = kc.strip()
        for _ in range(8):
            if kc in self.defines and kc not in KC_LABELS:
                kc = self.defines[kc].strip()
            else:
                break
        return kc

    def label(self, kc):
        """Return (tap, hold) labels. hold is '' when not a dual-role key."""
        kc = self.resolve(kc)
        if kc in KC_LABELS:
            return KC_LABELS[kc], ""
        if re.fullmatch(r"KC_[A-Z0-9]", kc):
            return kc[3:], ""
        if re.fullmatch(r"KC_F\d+", kc):
            return kc[3:], ""
        m = re.fullmatch(r"(\w+)_T\((.+)\)", kc)
        if m and m.group(1) in MOD_LABELS:
            return self.label(m.group(2))[0], MOD_LABELS[m.group(1)]
        m = re.fullmatch(r"LT\((\w+)\s*,\s*(.+)\)", kc)
        if m:
            return self.label(m.group(2))[0], self.layer_name(m.group(1))
        m = re.fullmatch(r"(MO|TG|TO|TT|OSL)\((\w+)\)", kc)
        if m:
            return self.layer_name(m.group(2)), m.group(1)
        m = re.fullmatch(r"OSM\((.+)\)", kc)
        if m:
            mods = [OSM_LABELS.get(p.strip(), p.strip()) for p in m.group(1).split("|")]
            if len(mods) > 2:
                return "Hyper", "1shot"
            return "+".join(mods), "1shot"
        m = re.fullmatch(r"(LCTL|RCTL|LSFT|RSFT|LALT|RALT|LGUI|RGUI|C|S|A|G)\((.+)\)", kc)
        if m:
            inner, _ = self.label(m.group(2))
            return f"{MOD_LABELS[m.group(1)]}+{inner}", ""
        return kc.replace("KC_", ""), ""

    def layer_name(self, sym):
        if sym in self.layer_index:
            return self.layer_names[self.layer_index[sym]]
        return sym.lstrip("_")

    def is_transparent(self, kc):
        return self.resolve(kc) in ("_______", "KC_TRNS")

    def effective(self, layer, idx):
        """Walk down through transparent keys to the base layer.
        Returns (keycode, from_layer)."""
        for li in range(layer, -1, -1):
            kc = self.layers[li][idx]
            if not self.is_transparent(kc):
                return kc, li
        return "KC_NO", 0


def load_geometry(path=KEYBOARD_JSON):
    import json
    d = json.loads(path.read_text())
    return d["layouts"]["LAYOUT"]["layout"]


# --------------------------------------------------------------------------
# raw HID
# --------------------------------------------------------------------------

def find_raw_hid_device():
    for sysdev in sorted(glob.glob("/sys/class/hidraw/hidraw*")):
        try:
            uevent = Path(sysdev, "device/uevent").read_text()
            m = re.search(r"HID_ID=\w+:(\w+):(\w+)", uevent)
            if not m or int(m.group(1), 16) != USB_VID or int(m.group(2), 16) != USB_PID:
                continue
            desc = Path(sysdev, "device/report_descriptor").read_bytes()
            if desc.startswith(RAW_USAGE_PAGE):
                return "/dev/" + os.path.basename(sysdev)
        except OSError:
            continue
    return None


class HidReader(threading.Thread):
    """Background thread: keeps the keyboard connection alive, calls
    on_layer(int) and on_status(str) from its own thread."""

    def __init__(self, on_layer, on_status):
        super().__init__(daemon=True)
        self.on_layer = on_layer
        self.on_status = on_status
        self.fd = None

    def run(self):
        while True:
            dev = find_raw_hid_device()
            if dev is None:
                self.on_status("keyboard not found")
                time.sleep(2)
                continue
            try:
                self.fd = os.open(dev, os.O_RDWR)
            except PermissionError:
                self.on_status(f"no permission for {dev} (install the udev rule)")
                time.sleep(5)
                continue
            except OSError as e:
                self.on_status(f"{dev}: {e.strerror}")
                time.sleep(2)
                continue
            self.on_status("connected")
            try:
                os.write(self.fd, bytes([MSG_LAYER]) + bytes(31))  # ask for current layer
                while True:
                    data = os.read(self.fd, 64)
                    if not data:
                        break
                    if data[0] == MSG_LAYER and len(data) > 1:
                        self.on_layer(data[1])
            except OSError:
                pass
            finally:
                try:
                    os.close(self.fd)
                except OSError:
                    pass
                self.fd = None
            self.on_status("disconnected")
            time.sleep(1)


# --------------------------------------------------------------------------
# rendering (cairo, shared by the GTK window and --render)
# --------------------------------------------------------------------------

THEME = {
    "bg": (0.11, 0.11, 0.13),
    "key": (0.19, 0.19, 0.22),
    "key_trans": (0.14, 0.14, 0.16),
    "key_border": (0.30, 0.30, 0.34),
    "text": (0.93, 0.93, 0.95),
    "text_dim": (0.55, 0.55, 0.60),
    "hold": (0.55, 0.80, 0.95),
    "accent": (0.95, 0.65, 0.25),
}


def canvas_size(geometry):
    w = max(k["x"] + k.get("w", 1) for k in geometry) * UNIT + 2 * PAD
    h = max(k["y"] + k.get("h", 1) for k in geometry) * UNIT + 2 * PAD + 34
    return int(w), int(h)


def rounded_rect(cr, x, y, w, h, r):
    import math
    cr.new_sub_path()
    cr.arc(x + w - r, y + r, r, -math.pi / 2, 0)
    cr.arc(x + w - r, y + h - r, r, 0, math.pi / 2)
    cr.arc(x + r, y + h - r, r, math.pi / 2, math.pi)
    cr.arc(x + r, y + r, r, math.pi, 3 * math.pi / 2)
    cr.close_path()


def draw(cr, keymap, geometry, layer, status):
    import gi
    gi.require_version("PangoCairo", "1.0")
    gi.require_version("Pango", "1.0")
    from gi.repository import Pango, PangoCairo

    w, h = canvas_size(geometry)
    cr.set_source_rgb(*THEME["bg"])
    cr.paint()

    def text(s, x, y, size, color, align="center", weight="normal"):
        layout = PangoCairo.create_layout(cr)
        layout.set_font_description(Pango.FontDescription(f"Sans {weight} {size}"))
        layout.set_text(s, -1)
        tw, th = layout.get_pixel_size()
        if align == "center":
            x -= tw / 2
        elif align == "right":
            x -= tw
        cr.set_source_rgb(*color)
        cr.move_to(x, y - th / 2)
        PangoCairo.show_layout(cr, layout)

    # header
    name = keymap.layer_names[layer] if layer < len(keymap.layer_names) else str(layer)
    text(f"Layer {layer}  ·  {name}", PAD, PAD - 4, 12, THEME["accent"], align="left", weight="bold")
    text(status, w - PAD, PAD - 4, 9, THEME["text_dim"], align="right")

    top = PAD + 30
    for idx, k in enumerate(geometry):
        x = PAD + k["x"] * UNIT + KEY_GAP / 2
        y = top + k["y"] * UNIT + KEY_GAP / 2
        kw = k.get("w", 1) * UNIT - KEY_GAP
        kh = k.get("h", 1) * UNIT - KEY_GAP
        kc, from_layer = keymap.effective(layer, idx)
        transparent = from_layer != layer and layer != 0
        tap, hold = keymap.label(kc)

        rounded_rect(cr, x, y, kw, kh, 6)
        cr.set_source_rgb(*(THEME["key_trans"] if transparent else THEME["key"]))
        cr.fill_preserve()
        cr.set_source_rgb(*THEME["key_border"])
        cr.set_line_width(1)
        cr.stroke()

        color = THEME["text_dim"] if transparent else THEME["text"]
        size = 13 if len(tap) <= 2 else (10 if len(tap) <= 6 else 8)
        cy = y + kh / 2 - (6 if hold else 0)
        text(tap, x + kw / 2, cy, size, color, weight="bold" if len(tap) <= 2 else "normal")
        if hold:
            text(hold, x + kw / 2, y + kh - 11, 7.5, THEME["text_dim"] if transparent else THEME["hold"])


# --------------------------------------------------------------------------
# GTK application
# --------------------------------------------------------------------------

def run_gtk(daemon):
    import gi
    gi.require_version("Gtk", "4.0")
    gi.require_version("Gdk", "4.0")
    from gi.repository import Gdk, Gio, GLib, Gtk

    keymap = Keymap()
    geometry = load_geometry()
    w, h = canvas_size(geometry)

    class App(Gtk.Application):
        def __init__(self):
            super().__init__(application_id=APP_ID,
                             flags=Gio.ApplicationFlags.HANDLES_COMMAND_LINE)
            self.window = None
            self.layer = 0
            self.status = "starting"

        def do_startup(self):
            Gtk.Application.do_startup(self)
            self.hold()  # keep the daemon alive with the window hidden
            self.build_window()
            HidReader(
                lambda l: GLib.idle_add(self.set_layer, l),
                lambda s: GLib.idle_add(self.set_status, s),
            ).start()

        def do_command_line(self, cmdline):
            args = cmdline.get_arguments()
            if "--daemon" not in args[1:]:
                self.toggle()
            return 0

        def build_window(self):
            self.window = Gtk.ApplicationWindow(application=self, title="Altair Keymap")
            self.window.set_default_size(w, h)
            self.window.set_resizable(False)
            self.window.set_hide_on_close(True)
            self.area = Gtk.DrawingArea()
            self.area.set_content_width(w)
            self.area.set_content_height(h)
            self.area.set_draw_func(lambda a, cr, aw, ah: draw(cr, keymap, geometry, self.layer, self.status))
            self.window.set_child(self.area)

            keys = Gtk.EventControllerKey()
            keys.connect("key-pressed", self.on_key)
            self.window.add_controller(keys)
            click = Gtk.GestureClick()
            click.connect("released", lambda *_: self.window.set_visible(False))
            self.area.add_controller(click)

        def on_key(self, ctrl, keyval, keycode, state):
            if keyval in (Gdk.KEY_Escape, Gdk.KEY_q):
                self.window.set_visible(False)
                return True
            return False

        def toggle(self):
            if self.window.get_visible():
                self.window.set_visible(False)
            else:
                self.window.present()

        def set_layer(self, layer):
            self.layer = layer
            self.area.queue_draw()

        def set_status(self, status):
            self.status = status
            self.area.queue_draw()

    app = App()
    sys.exit(app.run(sys.argv))


def render_png(out, layer):
    import cairo
    keymap = Keymap()
    geometry = load_geometry()
    w, h = canvas_size(geometry)
    surf = cairo.ImageSurface(cairo.FORMAT_ARGB32, w, h)
    draw(cairo.Context(surf), keymap, geometry, layer, "offline render")
    surf.write_to_png(out)
    print(f"wrote {out} ({w}x{h}), layers: {keymap.layer_names}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--daemon", action="store_true", help="start hidden; later launches toggle the window")
    ap.add_argument("--render", metavar="PNG", help="render a layer to PNG and exit")
    ap.add_argument("--layer", type=int, default=0)
    args = ap.parse_args()
    if args.render:
        render_png(args.render, args.layer)
        return
    run_gtk(args.daemon)


if __name__ == "__main__":
    main()

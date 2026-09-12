# Altair keymap visualizer

Floating window that shows the bryan keymap and follows the active layer live.
The firmware reports layer changes over raw HID (see `layer_state_set_user` in
`../keymap.c`); this app draws the layout from `../keymap.c` and the physical
geometry from `keyboard.json`.

## One-time setup

```sh
# 1. raw HID permission
sudo cp 70-altair-viz.rules /etc/udev/rules.d/
sudo udevadm control --reload && sudo udevadm trigger

# 2. flash firmware built with RAW_ENABLE (see ../rules.mk)
```

Deps (all already in Arch/Omarchy): python-gobject, gtk4, python-cairo.

## Usage

```sh
./altair_viz.py --daemon         # started from Hyprland autostart, hidden
./altair_viz.py                  # toggle the window (bound to SUPER+SHIFT+K)
./altair_viz.py --render out.png --layer 1   # offline render for debugging
```

Esc, q, or a click hides the window. The daemon keeps the HID connection open
and reconnects if the keyboard is unplugged.

Transparent keys (`_______`) are drawn dimmed with the key they fall through to.

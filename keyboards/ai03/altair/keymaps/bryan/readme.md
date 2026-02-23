# Bryan's Altair Keymap

Ported from ZMK config for Ogre Ergo keyboard.

## How to Reflash

The Altair is a **split RP2040 keyboard**. Each half must be flashed separately.

### Quick Steps

1. Run flash command (it waits for bootloader):
   ```bash
   qmk flash -kb ai03/altair -km bryan
   ```
2. Enter bootloader on the connected half (see below)
3. Firmware auto-copies to the RP2040 USB drive
4. Unplug, plug in the **other half**, repeat steps 1-2

### Entering Bootloader Mode

Each half can enter bootloader independently with a **2-key combo**:

| Half | Combo | Layer key | Boot key |
|------|-------|-----------|----------|
| **Left** | Hold **NUM** (left thumb) + press **top-left corner** (`=` position) | `SL_NUM` at `[4,5]` | `QK_BOOT` on _NUM `[0,0]` |
| **Right** | Hold **SYM** (right thumb) + press **top-right corner** (`-` position) | `SL_SYM` at `[9,1]` | `QK_BOOT` on _SYM `[5,6]` |

The board shows up as a USB drive named `RPI-RP2` when in bootloader.

### Full Example (both halves)

```bash
# 1. Plug USB into LEFT half
qmk flash -kb ai03/altair -km bryan
# Hold NUM + top-left key → flashes automatically

# 2. Plug USB into RIGHT half
qmk flash -kb ai03/altair -km bryan
# Hold SYM + top-right key → flashes automatically
```

### Compile Only (no flash)

```bash
qmk compile -kb ai03/altair -km bryan
```

Output: `.build/ai03_altair_bryan.uf2`

## Features

- **5 Layers**: Default, Symbols, Numbers, Navigation, Settings
- **Custom Mod-Tap**: 250ms tapping term, 150ms quick tap (matches ZMK behavior)
- **Home Row Mods**: Bottom row Z/X/C and Comma/Period/Slash
- **Layer Taps**: Space (NUM layer), Enter (SYM layer)
- **Conditional Layer**: When SYM + NUM are both active, NAV layer activates
- **Sticky Shift**: One-shot right shift for single-key shifted characters
- **Apple Globe Key**: `AP_GLOB` sends `AC_NEXT_KEYBOARD_LAYOUT_SELECT` (0x029D) — triggers emoji picker / dictation on macOS
- **macOS Optimized**: Mission Control, media controls

## Layer Overview

### 0: Default
- QWERTY layout with mod-tap on bottom row
- Globe key on right inner bottom row (second B position)
- Escape on left home position
- Backspace on left inner bottom row

### 1: Symbols (_SYM)
- Activated by holding Enter or SYM thumb key
- Function keys F1-F12
- Symbols: !@#$%^&*()_+
- Brackets, braces, parentheses
- Arrow keys on right hand
- **QK_BOOT at top-right corner**

### 2: Numbers (_NUM)
- Activated by holding Space or NUM thumb key
- Number row duplicated
- Function keys F1-F12
- Numpad layout on right hand (4/5/6, 1/2/3)
- **QK_BOOT at top-left corner**

### 3: Navigation (_NAV)
- Automatically activates when both SYM + NUM are held
- Arrow keys
- Page Up/Down, Home/End
- Tab/Shift+Tab for window switching
- Media controls (volume, play/pause)
- Browser back/forward

### 4: Settings (_SETTINGS)
- Accessed via NAV layer + N key
- QK_BOOT at multiple positions (legacy access)

## ZMK → QMK Translation Notes

### Direct Equivalents
- `&kp` → `KC_` keycodes
- `&mt` → `MT()` or `*_T()` mod-tap macros
- `&lt` → `LT()` layer tap
- `&sk` → `OSM()` one-shot modifier
- `&sl` → `MO()` momentary layer (ZMK's sticky layer → QMK's momentary)

### Differences
1. **Sticky Layer**: ZMK's `&sl` becomes `MO()` in QMK (stays active while held, not sticky)
2. **Bluetooth/Power**: Settings layer simplified (no BT/USB switching needed for wired board)
3. **Globe Key**: Custom `AP_GLOB` keycode sends Apple Globe via consumer HID usage
4. **Mission Control**: Mapped to Cmd+Tab (closest equivalent)

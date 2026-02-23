# Bryan's Altair QMK Setup

## Quick Reference

### 📝 Edit Your Keymap
```bash
# Open keymap in your editor
code keyboards/ai03/altair/keymaps/bryan/keymap.c

# Or use any editor:
vim keyboards/ai03/altair/keymaps/bryan/keymap.c
nano keyboards/ai03/altair/keymaps/bryan/keymap.c
```

### 🔨 Compile After Edits
```bash
# From anywhere:
cd /Users/bryanarendt/code2/qmk_firmware
./compile-altair.sh

# Or manually:
qmk compile -kb ai03/altair -km bryan
```

### 📁 File Locations
- **Keymap**: `keyboards/ai03/altair/keymaps/bryan/keymap.c`
- **Config**: `keyboards/ai03/altair/keymaps/bryan/config.h`
- **Rules**: `keyboards/ai03/altair/keymaps/bryan/rules.mk`
- **Firmware**: `ai03_altair_bryan.uf2` (created after compilation)

### 🔄 Flash to Keyboard

**Bootmagic Method:**
1. Unplug keyboard
2. Hold ESC (left) or = key (right inner column)
3. Plug in USB while holding
4. Release key
5. Drag `ai03_altair_bryan.uf2` to RPI-RP2 drive

**Reset Key Method (after first flash):**
1. Hold Space + Enter (activates NAV via conditional layer)
2. While holding both, press N (activates SETTINGS layer)
3. Tap ESC/Q (left) or P/BSPC (right) to enter bootloader
4. Drag `ai03_altair_bryan.uf2` to RPI-RP2 drive

### 🎹 Current Layout

**Base Layer:**
```
Row 1: =  1  2  3  4  5  `  PGUP   6  7  8  9  0  -
Row 2: TAB Q  W  E  R  T  ]  [     Y  U  I  O  P  \
Row 3: ESC A  S  D  F  G  PGDN '   H  J  K  L  ;  '"
Row 4: SHFT Z† X† C† V  B  __  __  N  M  ,† .† /† SHFT†
Thumb:     `† ALT NUM SPC  BSPC    ENT RALT SYM

† = Mod-tap or special behavior
```

**Layer Access:**
- SYM (symbols): Hold right Enter
- NUM (numbers): Hold left Space
- NAV (navigation): Hold both Space + Enter (conditional)
- SETTINGS: NAV layer + N, then use reset keys

### 🛠️ Common Edits

**Change a key:**
```c
// In keymap.c, find the layer and position:
KC_A,     // Change to KC_B, KC_ESC, etc.
```

**Change mod-tap timing:**
```c
// In config.h:
#define TAPPING_TERM 250  // Increase for slower, decrease for faster
```

**Add a new layer:**
```c
// 1. Add enum in keymap.c:
enum layers {
    _DEFAULT,
    _SYM,
    _NUM,
    _NAV,
    _SETTINGS,
    _MYNEWLAYER  // Add here
};

// 2. Add layer definition:
[_MYNEWLAYER] = LAYOUT(
    // ... 64 keys ...
),
```

### 📚 QMK Keycodes Reference
- **Basic**: `KC_A` to `KC_Z`, `KC_1` to `KC_0`
- **Modifiers**: `KC_LCTL`, `KC_LSFT`, `KC_LALT`, `KC_LGUI`
- **Special**: `KC_ESC`, `KC_TAB`, `KC_SPC`, `KC_ENT`, `KC_BSPC`, `KC_DEL`
- **Nav**: `KC_UP`, `KC_DOWN`, `KC_LEFT`, `KC_RGHT`, `KC_PGUP`, `KC_PGDN`
- **Function**: `KC_F1` to `KC_F12`
- **Mod-tap**: `LCTL_T(KC_A)` = Ctrl when held, A when tapped
- **Layer tap**: `LT(1, KC_SPC)` = Layer 1 when held, Space when tapped
- **One-shot**: `OSM(MOD_LSFT)` = Sticky shift

Full reference: https://docs.qmk.fm/keycodes

### 🔍 Troubleshooting

**Compilation errors:**
- Check syntax: commas between keys, semicolons after layers
- Count keys: Each layer needs exactly 64 keys (14+14+14+14+8)
- Check defines: Make sure all custom keycodes are defined

**Keyboard not recognized:**
- Check USB cable
- Try different USB port
- Verify both halves are flashed with same firmware

**Key not working:**
- Check keymap - is it mapped to the right position?
- Test in default layer first
- Use QMK Configurator to visualize: https://config.qmk.fm/#/test

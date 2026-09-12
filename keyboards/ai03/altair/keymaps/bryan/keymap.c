/* Copyright 2024 ai03 Design Studio */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include QMK_KEYBOARD_H
#include "raw_hid.h"

// Custom keycodes
enum custom_keycodes {
    AP_GLOB = SAFE_RANGE,
};

// Layer names for readability
enum layers {
    _DEFAULT,
    _SYM,
    _NUM,
    _NAV,
    _SETTINGS
};

// Custom mod-tap configuration matching ZMK behavior
#define MY_TAPPING_TERM 250
#define MY_QUICK_TAP_TERM 150

// Define custom mod-tap keys matching your ZMK layout
#define MT_Z    LCTL_T(KC_Z)
#define MT_X    LGUI_T(KC_X)
#define MT_C    LALT_T(KC_C)
#define MT_COMM RALT_T(KC_COMM)
#define MT_DOT  RGUI_T(KC_DOT)
#define MT_SLSH RCTL_T(KC_SLSH)
#define MT_QUOT RGUI_T(KC_QUOT)
#define MT_GRV  LCTL_T(KC_GRV)

// Layer taps
#define LT_SPC  LT(_NUM, KC_SPC)
#define LT_ENT  LT(_SYM, KC_ENT)
#define SL_NUM  MO(_NUM)
#define SL_SYM  MO(_SYM)

// Sticky shift (using One Shot Shift)
#define SK_RSFT OSM(MOD_RSFT)

// macOS specific keys
#define KC_MISS  LGUI(KC_TAB)  // Mission Control approximation
#define KC_LAUNCH LGUI(KC_SPC) // Launchpad approximation

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_DEFAULT] = LAYOUT(
        KC_EQL,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_GRV,  KC_PGUP, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_RBRC, KC_LBRC, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_PGDN, KC_QUOT, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, MT_QUOT,
        KC_LSFT, MT_Z,    MT_X,    MT_C,    KC_V,    KC_B,    KC_BSPC, AP_GLOB, KC_N,    KC_M,    MT_COMM, MT_DOT,  MT_SLSH, SK_RSFT,
                                   KC_LEFT, KC_RGHT, SL_NUM,  LT_SPC,  LT_ENT, SL_SYM,  KC_DOWN, KC_UP
    ),

    [_SYM] = LAYOUT(
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  QK_BOOT,
        KC_PLUS, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_VOLU, _______, _______, KC_CIRC, KC_MPRV, KC_MPLY, KC_MNXT, KC_RPRN, KC_UNDS,
        KC_PGUP, KC_PGDN, KC_SLSH, KC_BSLS, KC_LPRN, KC_VOLD, KC_END,  KC_HOME, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_UNDS, KC_DEL,
        _______, KC_GRV,  KC_LCBR, KC_RCBR, KC_LBRC, KC_MUTE, KC_INS,  _______, KC_TILD, KC_PIPE, KC_LCBR, KC_RCBR, OSM(MOD_RCTL), _______,
                                   _______, _______, _______, KC_BSPC, _______, _______, _______, _______
    ),

    [_NUM] = LAYOUT(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_EQL,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    _______, _______, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______, _______, KC_F12,  KC_4,    KC_5,    KC_6,    XXXXXXX, _______,
        _______, LCTL_T(KC_F6), LGUI_T(KC_F7), LALT_T(KC_F8), KC_F9, KC_F10,  _______, _______, KC_F11,  KC_1,    RALT_T(KC_2), RGUI_T(KC_3), KC_RCTL, _______,
                                   _______, _______, _______, KC_BSPC, _______, _______, _______, _______
    ),

    [_NAV] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        XXXXXXX, KC_TAB,  S(KC_TAB), KC_MISS, KC_LAUNCH, KC_VOLU, _______, _______, OSM(MOD_LALT | MOD_LCTL | MOD_LGUI | MOD_LSFT), KC_PGDN, KC_PGUP, XXXXXXX, KC_BSPC, _______,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_VOLD, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_ENT,  _______,
        _______, C(S(KC_H)), C(S(KC_L)), KC_WBAK, KC_WFWD, KC_MPLY, _______, _______, MO(_SETTINGS), KC_END, KC_HOME, XXXXXXX, KC_DEL,  _______,
                                   _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [_SETTINGS] = LAYOUT(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, QK_BOOT,
        XXXXXXX, QK_BOOT, XXXXXXX, XXXXXXX, _______, XXXXXXX, _______, _______, _______, _______, _______, _______, QK_BOOT, _______,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______, _______, _______, _______, _______
    )
};

// ---------------------------------------------------------------------------
// Raw HID layer reporting for the host-side keymap visualizer.
// Host -> keyboard: [0x01]         request current layer
// Keyboard -> host: [0x01, layer]  sent on every layer change and on request
// Keyboard -> host: [0x02, row, col, pressed, layer]  sent on every key event
// ---------------------------------------------------------------------------
#define VIZ_MSG_LAYER 0x01
#define VIZ_MSG_KEY   0x02

static void viz_send_layer(layer_state_t state) {
    uint8_t report[32] = {0};  // RAW_EPSIZE
    report[0] = VIZ_MSG_LAYER;
    report[1] = get_highest_layer(state);
    raw_hid_send(report, sizeof(report));
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (length > 0 && data[0] == VIZ_MSG_LAYER) {
        viz_send_layer(layer_state | default_layer_state);
    }
}

static void viz_send_key(keyrecord_t *record) {
    uint8_t report[32] = {0};  // RAW_EPSIZE
    report[0] = VIZ_MSG_KEY;
    report[1] = record->event.key.row;
    report[2] = record->event.key.col;
    report[3] = record->event.pressed;
    report[4] = get_highest_layer(layer_state | default_layer_state);
    raw_hid_send(report, sizeof(report));
}

// Apple Globe key handler
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    viz_send_key(record);
    switch (keycode) {
        case AP_GLOB:
            host_consumer_send(record->event.pressed ? AC_NEXT_KEYBOARD_LAYOUT_SELECT : 0);
            return false;
    }
    return true;
}

// Configure tapping term for mod-tap keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT_Z:
        case MT_X:
        case MT_C:
        case MT_COMM:
        case MT_DOT:
        case MT_SLSH:
        case MT_QUOT:
        case MT_GRV:
        case LCTL_T(KC_F6):
        case LGUI_T(KC_F7):
        case LALT_T(KC_F8):
        case RALT_T(KC_2):
        case RGUI_T(KC_3):
            return MY_TAPPING_TERM;
        default:
            return TAPPING_TERM;
    }
}

// Implement conditional layer: when SYM + NUM are both active, activate NAV.
// Then report the resulting layer to the host-side visualizer.
layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, _SYM, _NUM, _NAV);
    viz_send_layer(state | default_layer_state);
    return state;
}

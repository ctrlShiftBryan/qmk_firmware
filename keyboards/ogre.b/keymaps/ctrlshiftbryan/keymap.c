#include QMK_KEYBOARD_H

extern keymap_config_t keymap_config;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QWERTY 0
#define _SYMB 1
#define _NAV 2
#define _ADJUST 3

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  SYMB,
  NAV,
  ADJUST,
};

// Shortcut to make keymap more readable
#define KC_BKSL KC_BSLASH
#define SYM_L   MO(_SYMB)

#define KC_ALAS LALT_T(KC_PAST)
#define KC_CTPL LCTL_T(KC_PSLS)

#define KC_NAGR LT(_NAV, KC_GRV)
#define KC_NAMI LT(_NAV, KC_MINS)

#define KC_ADEN LT(_ADJUST, KC_END)
#define KC_ADPU LT(_ADJUST, KC_PGUP)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT
(
		KC_EQL, KC_1, KC_2, KC_3, KC_4, KC_5, KC_GRV,
		KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_RCBR,
		KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_NO,
		KC_ESC, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,
		KC_LCTL, KC_NO, KC_LEFT, KC_RGHT, KC_LALT, KC_SPC, KC_BSPC,
		KC_NO, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS,
		KC_LCBR, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS,
		KC_QUOT, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_LCTL,
		KC_N, KC_M, KC_COMM, KC_DOT, KC_QUES, KC_LSFT, KC_DEL,
		KC_LALT, KC_ENT, KC_LALT, KC_UP, KC_DOWN, KC_NO, KC_MENU)

};

/* Copyright 2019 Yiancar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

// KVM keyboard, same behaviour as the M6-B remote. The hardware KVM
// selects a port on Scroll Lock, Scroll Lock, <port number>; keys 1-8 on
// the number row send that whole sequence. 9, 0, - and = are guesses at
// the buzzer-off command (Scroll Lock + F11 did not work on this switch).
// The number row's plain characters move to Fn, and F10-F12 to Fn + P [ ].
// The backlight is dark except the last KVM key pressed: cyan for a port,
// red for a buzzer guess.
enum custom_keycodes {
    KVM_1 = SAFE_RANGE,
    KVM_2,
    KVM_3,
    KVM_4,
    KVM_5,
    KVM_6,
    KVM_7,
    KVM_8,
    BEEP_B,     // Scroll Lock, Scroll Lock, B
    BEEP_B0,    // Scroll Lock, Scroll Lock, B, 0
    BEEP_M,     // Scroll Lock, Scroll Lock, M
    BEEP_CTF11, // Ctrl, Ctrl, F11
};

// The switch misses presses that arrive back to back, so space them out.
#define KVM_GAP_MS 50

// 25% of full brightness.
#define LED_V 64

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_65_ansi( /* Base */
        QK_GESC, KVM_1,   KVM_2,   KVM_3,   KVM_4,   KVM_5,   KVM_6,   KVM_7,   KVM_8,   BEEP_B,  BEEP_B0, BEEP_M,  BEEP_CTF11, KC_BSPC, KC_HOME,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGDN,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          KC_UP,   KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                             KC_RALT, MO(1),   KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [1] = LAYOUT_65_ansi( /* FN */
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_DEL,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_F10,  KC_F11,  KC_F12,  QK_BOOT, _______,
        _______, _______, KC_SCRL, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
        KC_VOLU, KC_VOLD, KC_MUTE,                   _______,                            _______, _______, _______, _______, _______, _______
    )
};

// LED index of the number-row keys: Esc is 0, then 1, 2, ... in order
// (g_is31fl3733_leds in nk65.c).
#define LED_NUMROW(n) (n)

static uint8_t lit_led = 255; // none until the first KVM key
static rgb_t   lit_rgb;

#define CYAN 128
#define RED  0

// `prefix` twice, then each of `keys`, with gaps between every tap.
// Lights the number-row key `key` (1 = the 1 key, 12 = the = key).
static void kvm_send(uint16_t prefix, const uint16_t *keys, uint8_t n, uint8_t key, uint8_t hue) {
    tap_code(prefix);
    wait_ms(KVM_GAP_MS);
    tap_code(prefix);
    for (uint8_t i = 0; i < n; i++) {
        wait_ms(KVM_GAP_MS);
        tap_code(keys[i]);
    }

    lit_led = LED_NUMROW(key);
    lit_rgb = hsv_to_rgb((hsv_t){.h = hue, .s = 255, .v = LED_V});
}

#define SEND(prefix, key, hue, ...)                                        \
    do {                                                                   \
        const uint16_t k[] = {__VA_ARGS__};                                \
        kvm_send(prefix, k, sizeof(k) / sizeof(k[0]), key, hue);           \
    } while (0)

void keyboard_post_init_user(void) {
    // Black base frame for the indicator below. Not saved, so whatever the
    // stock firmware or VIA left in EEPROM does not matter.
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, 0);
}

bool rgb_matrix_indicators_user(void) {
    rgb_matrix_set_color_all(0, 0, 0);
    if (lit_led != 255) {
        rgb_matrix_set_color(lit_led, lit_rgb.r, lit_rgb.g, lit_rgb.b);
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode >= KVM_1 && keycode <= KVM_8) {
        if (record->event.pressed) {
            uint8_t port = keycode - KVM_1 + 1;
            SEND(KC_SCRL, port, CYAN, KC_1 + port - 1);
        }
        return false;
    }
    if (!record->event.pressed) return true;
    switch (keycode) {
        case BEEP_B:     SEND(KC_SCRL, 9, RED, KC_B);        return false;
        case BEEP_B0:    SEND(KC_SCRL, 10, RED, KC_B, KC_0); return false;
        case BEEP_M:     SEND(KC_SCRL, 11, RED, KC_M);       return false;
        case BEEP_CTF11: SEND(KC_LCTL, 12, RED, KC_F11);     return false;
    }
    return true;
}

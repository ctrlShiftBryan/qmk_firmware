#include QMK_KEYBOARD_H
#include "keyboards/wilba_tech/wt_rgb_backlight.h"

// KVM remote. The switch selects a port on Scroll Lock, Scroll Lock,
// <port number>. Each key sends that whole sequence with a top-row
// digit: a tap selects ports 1-6, a double tap on the two left keys
// selects 7 and 8. The last key pressed stays lit: cyan for a tap,
// red for a double tap.
enum custom_keycodes {
    KVM_2 = SAFE_RANGE,
    KVM_3,
    KVM_5,
    KVM_6,
};

enum tap_dances {
    TD_1_7,
    TD_4_8,
};

// The switch misses presses that arrive back to back, so space them out.
#define KVM_GAP_MS 50

// 25% of full brightness.
#define LED_V 64

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT(
        TD(TD_1_7), KVM_2, KVM_3,
        TD(TD_4_8), KVM_5, KVM_6),
};

// LED index of each key, in LAYOUT order (see wt_rgb_backlight.c).
static const uint8_t key_led[6] = {0, 3, 5, 1, 2, 4};

extern backlight_config g_config;
void backlight_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
void backlight_set_color_all(uint8_t red, uint8_t green, uint8_t blue);

static uint8_t lit_led = 255; // none until the first press
static rgb_t   lit_rgb;

static void kvm_select(uint8_t port, uint8_t key, uint8_t hue) {
    tap_code(KC_SCRL);
    wait_ms(KVM_GAP_MS);
    tap_code(KC_SCRL);
    wait_ms(KVM_GAP_MS);
    tap_code(KC_1 + port - 1);

    lit_led = key_led[key - 1];
    lit_rgb = hsv_to_rgb((hsv_t){.h = hue, .s = 255, .v = LED_V});
}

#define CYAN 128
#define RED  0

static void td_1_7(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) kvm_select(1, 1, CYAN);
    else                   kvm_select(7, 1, RED);
}

static void td_4_8(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) kvm_select(4, 4, CYAN);
    else                   kvm_select(8, 4, RED);
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_1_7] = ACTION_TAP_DANCE_FN(td_1_7),
    [TD_4_8] = ACTION_TAP_DANCE_FN(td_4_8),
};

void keyboard_post_init_user(void) {
    // Effect "all off" as the base frame. Set in RAM at boot so whatever
    // the stock firmware left in EEPROM (factory test pattern, VIA
    // settings) does not matter.
    g_config.effect = 0;
}

// Runs after every backlight frame and overrides it.
void backlight_effect_indicators(void) {
    backlight_set_color_all(0, 0, 0);
    if (lit_led != 255) {
        backlight_set_color(lit_led, lit_rgb.r, lit_rgb.g, lit_rgb.b);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) return true;
    switch (keycode) {
        case KVM_2: kvm_select(2, 2, CYAN); return false;
        case KVM_3: kvm_select(3, 3, CYAN); return false;
        case KVM_5: kvm_select(5, 5, CYAN); return false;
        case KVM_6: kvm_select(6, 6, CYAN); return false;
    }
    return true;
}

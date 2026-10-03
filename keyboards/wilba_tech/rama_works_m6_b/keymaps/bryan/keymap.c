#include QMK_KEYBOARD_H

// KVM remote. The switch selects a port on Ctrl, Ctrl, <port number>.
// Each port key sends that whole sequence with the right Ctrl and a
// top-row digit. Hold the top-left key to reach ports 5-8.
enum custom_keycodes {
    KVM_1 = SAFE_RANGE,
    KVM_2,
    KVM_3,
    KVM_4,
    KVM_5,
    KVM_6,
    KVM_7,
    KVM_8,
};

// The switch misses presses that arrive back to back, so space them out.
#define KVM_GAP_MS 50

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Ports 1-4
    LAYOUT(
        MO(1),  KVM_1, KVM_2,
        KC_NUM, KVM_3, KVM_4),
    // Ports 5-8, while the top-left key is held
    LAYOUT(
        _______, KVM_5, KVM_6,
        KC_NUM,  KVM_7, KVM_8),
};

static void kvm_select(uint8_t port) {
    tap_code(KC_RCTL);
    wait_ms(KVM_GAP_MS);
    tap_code(KC_RCTL);
    wait_ms(KVM_GAP_MS);
    tap_code(KC_1 + port - 1);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode >= KVM_1 && keycode <= KVM_8) {
        if (record->event.pressed) {
            kvm_select(keycode - KVM_1 + 1);
        }
        return false;
    }
    return true;
}

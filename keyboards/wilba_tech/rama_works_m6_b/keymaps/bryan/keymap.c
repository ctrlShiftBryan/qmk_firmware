#include QMK_KEYBOARD_H

enum custom_keycodes {
    AP_GLOB = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT(
        AP_GLOB, AP_GLOB, AP_GLOB, AP_GLOB, AP_GLOB, AP_GLOB),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case AP_GLOB:
            host_consumer_send(record->event.pressed ? AC_NEXT_KEYBOARD_LAYOUT_SELECT : 0);
            return false;
    }
    return true;
}

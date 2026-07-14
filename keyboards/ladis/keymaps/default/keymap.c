// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

enum layers {
  _BL,
  _FL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_BL] = LAYOUT(
		KC_WWW_BACK, KC_UP, KC_WWW_FORWARD, \
		KC_LEFT, KC_DOWN, KC_RIGHT, \
		MO(_FL), KC_WWW_REFRESH, KC_F16, \
		LCTL(KC_LGUI), KC_MUTE),

[_FL] = LAYOUT(
		KC_F17, KC_F18, KC_F19, \
		KC_F20, KC_F21, KC_F22, \
		KC_NO, KC_F23, KC_F24, \
		KC_SYSTEM_SLEEP, KC_MUTE),

};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_LEFT, KC_RIGHT), ENCODER_CCW_CW(KC_VOLD, KC_VOLU),},
    [1] = { ENCODER_CCW_CW(MS_WHLD, MS_WHLU), ENCODER_CCW_CW(MS_WHLL, MS_WHLR),}
};
#endif

bool oled_task_user(void) {
    // write custom code here to edit OLED display and return false at end.
    return true; // return false to disable logo and layer indicator
}


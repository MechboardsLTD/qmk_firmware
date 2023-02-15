// Copyright 2023 Dasky (@daskygit)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum keymap_layers { _NUM, _MOV, _ADJ};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_NUM] = LAYOUT(
        KC_1          , KC_2    , LT(_ADJ,KC_3),
        KC_4          , KC_5    , KC_6,
        KC_7          , KC_8    , KC_9,
        LT(0,KC_ENTER), KC_0    , KC_DOT
    ),
    [_MOV] = LAYOUT(
        KC_INS       , KC_HOME  , LT(_ADJ,KC_PGUP),
        KC_DEL       , KC_END   , KC_PGDN,
        KC_BSPC      , KC_UP    , KC_ENT,
        LT(0,KC_LEFT), KC_DOWN  , KC_RIGHT
    ),
    [_ADJ] = LAYOUT(
        QK_BOOT      , KC_NO    , KC_TRNS,
        RGB_HUI      , RGB_SAI  , RGB_VAI,
        RGB_HUD      , RGB_SAD  , RGB_VAD,
        RGB_TOG      , RGB_MOD  , RGB_RMOD
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [_NUM] =  {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_MOV] =  {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)},
    [_ADJ] =  {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)}
};
#endif
// clang-format on

bool some_key_pressed = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        some_key_pressed = true;
    }
    switch (keycode) {
        case LT(0, KC_ENTER):
            if (!record->tap.count && record->event.pressed) {
                layer_move(_MOV);
                return false;
            }
            return true;
        case LT(0, KC_LEFT):
            if (!record->tap.count && record->event.pressed) {
                layer_move(_NUM);
                return false;
            }
            return true;
    }
    return true;
}

enum oled_display_modes { oled_mode_default = 1, oled_mode_off, oled_mode_bongocat };

uint8_t current_oled_display_mode = oled_mode_default;

extern void render_bongocat(void);

bool oled_task_user(void) {
    switch (current_oled_display_mode) {
        case oled_mode_bongocat:
            if (oled_on()) {
                render_bongocat();
            };
            return false;
        case oled_mode_off:
            oled_clear();
            return false;
        default:
            return true;
    }
}

enum via_custom_oled_value { id_oled_display_mode = 1 };

void custom_oled_config_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_oled_display_mode: {
            current_oled_display_mode = *value_data;
            switch (current_oled_display_mode) {
                case oled_mode_bongocat:
                    oled_init(OLED_ROTATION_0);
                    break;
                default:
                    oled_init(OLED_ROTATION_270);
                    break;
            }
            break;
        }
    }
}

void custom_oled_config_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_oled_display_mode: {
            *value_data = current_oled_display_mode;
            break;
        }
    }
}

void custom_oled_config_config_save(void) {
    // Don't save
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                custom_oled_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                custom_oled_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                custom_oled_config_config_save();
                break;
            }
            default: {
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }
}
// Copyright 2025 Dasky (@daskygit)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"

#ifdef OLED_ENABLE
void render_logo(void) {
    static const char PROGMEM pro_logo[] = {
        252, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 31,  31,  31,  31, 31, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 0,   0,   0,   248, 248, 0,   0,   0,   0,   0,   0,   0,   0,   128, 128, 0,   0,   224, 224, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   248, 248, 24,  24,  24,  24,  24,  24,  0,   0,   240, 248, 24, 24, 24, 24, 24, 248, 240, 0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,   255, 255, 255, 255, 255, 7,   7,  7,  7,  7,  255, 7,   7,   7,   7,   7,   255, 7, 7, 7, 7, 7, 255, 7,  7,  7,  7,  7,  255, 7,  7,  7,  7,  7,  255, 255, 255, 4,   4,   4,   4,   4,   255,
        7,   7,   7,   7,   7,   255, 7,   7,   7,   7,   7,   255, 255, 255, 255, 255, 0,   0,   0,   255, 255, 0,   0,   0,   0,   0,   0,   0,   0,   249, 249, 0,   0,   255, 255, 0,   0,   248, 248, 0,   0,  0,  0,   248, 248, 0,   0,   7,   7,   6,   6,   6,   6,   254, 252, 0,   0,   251, 255, 6,   6,   6,   6,   6,   255, 251, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   255, 255, 255, 255, 255, 65,  65,  65,  65,  65,  255, 255, 255, 255, 255, 255, 255, 65, 65, 65, 65, 65, 255, 255, 255, 255, 255, 255, 255, 65, 65, 65, 65, 65, 255, 255, 255, 65, 65, 65, 65, 65, 255, 127, 127, 127, 127, 127, 255, 65, 65, 65, 65, 65,  255, 255, 255, 255, 255, 0,   0, 0, 3, 3, 3, 3,   3,  3,  3,  3,  0,  0,   3,  3,  0,  0,  3,  3,   0,   0,   131, 131, 131, 131, 131, 131,
        255, 255, 0,   0,   3,   3,   3,   3,   3,   3,   3,   1,   0,   0,   1,   3,   3,   3,   3,   3,   3,   3,   225, 240, 48,  176, 176, 112, 240, 48,  176, 176, 112, 240, 112, 176, 176, 112, 240, 224, 0,  0,  63,  127, 255, 255, 255, 240, 240, 240, 240, 240, 255, 255, 255, 255, 255, 255, 255, 240, 240, 240, 240, 240, 255, 255, 255, 255, 255, 255, 255, 240, 240, 240, 240, 240, 255, 255, 255, 240, 240, 240, 240, 240, 255, 240, 240, 240, 240, 240, 255, 240, 240, 240, 240, 240, 255, 255, 255, 127, 63,  0,   0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   1,  1,  1,  1,  1,  1,   1,   1,   0,   0,   0,   0,   0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 31,  63, 48, 61, 61, 62, 63,  48, 61, 61, 50, 63, 56,  55,  55,  56,  63,  31,  0,   0,
    };
    oled_write_raw_P(pro_logo, sizeof(pro_logo));
}
#endif

const pin_t row_pins_left[MATRIX_ROWS_PER_HAND]  = MATRIX_ROW_PINS;
const pin_t row_pins_right[MATRIX_ROWS_PER_HAND] = MATRIX_ROW_PINS_RIGHT;
const pin_t col_pins_left[MATRIX_COLS]  = MATRIX_COL_PINS;
const pin_t col_pins_right[MATRIX_COLS] = MATRIX_COL_PINS_RIGHT;

const uint32_t       *row_pins          = NULL;
const uint32_t       *col_pins          = NULL;

void matrix_init_kb(void){

    gpio_set_pin_input_high(ENCODER_SW_PIN);

    if (is_keyboard_left()) {
        row_pins = row_pins_left;
        col_pins = col_pins_left;
    } else {
        row_pins = row_pins_right;
        col_pins = col_pins_right;
    }
    matrix_init_user();
}

void matrix_output_unselect_delay(uint8_t line, bool key_pressed) {
    if (key_pressed){
        bool done = false;
        while (done == false) {
            bool cols_high = true;
            for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
                if (gpio_read_pin(col_pins[col_index]) == 0){
                    cols_high = false;
                }
            }
            if (cols_high){
                done = true;
            }
        }
    }
}

void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    ATOMIC_BLOCK_FORCEON {
        gpio_set_pin_output(row_pins[current_row]);
        gpio_write_pin_low(row_pins[current_row]);
    }
    while (gpio_read_pin(row_pins[current_row])!= 0){

    };

    // For each col...
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        uint8_t pin_state = gpio_read_pin(col_pins[col_index]);
        if (col_index == 5 && current_row == 4){
            if (gpio_read_pin(ENCODER_SW_PIN) == 0){
                pin_state = 0;
            }
        }
        current_row_value |= pin_state ? 0 : row_shifter;
    }

    // Unselect row
    ATOMIC_BLOCK_FORCEON {
        gpio_set_pin_input_high(row_pins[current_row]);
    }
    matrix_output_unselect_delay(current_row, current_row_value != 0); // wait for all Col signals to go HIGH

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}
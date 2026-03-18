// Copyright 2025 Dasky (@daskygit)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "common/display_lcd.h"

void keyboard_post_init_kb(void) {
    display_init_kb();
    keyboard_post_init_user();
}

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

        if (col_index == 6 && current_row == 4){
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

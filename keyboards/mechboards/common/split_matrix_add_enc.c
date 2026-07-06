// Copyright 2026 Dasky (@daskygit)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gpio.h"
#include "split_util.h"
#include "atomic_util.h"
#include "keyboard.h"

#if !defined(ENCODER_SW_PIN) || !defined(ENCODER_SW_COL) || !defined(ENCODER_SW_ROW)
#   error "ENCODER_SW_PIN, ENCODER_SW_COL, and ENCODER_SW_ROW must be defined in config.h to use encoder switch functionality"
#endif

#if !defined(ENCODER_SW_PIN_RIGHT)
#   define ENCODER_SW_PIN_RIGHT ENCODER_SW_PIN
#endif

#if !defined(ENCODER_SW_COL_RIGHT) || !defined(ENCODER_SW_ROW_RIGHT)
#   error "ENCODER_SW_COL_RIGHT, and ENCODER_SW_ROW_RIGHT must be defined in config.h to use encoder switch functionality"
#endif


const pin_t row_pins_left[MATRIX_ROWS/2]  = MATRIX_ROW_PINS;
const pin_t row_pins_right[MATRIX_ROWS/2] = MATRIX_ROW_PINS_RIGHT;
const pin_t col_pins_left[MATRIX_COLS]  = MATRIX_COL_PINS;
const pin_t col_pins_right[MATRIX_COLS] = MATRIX_COL_PINS_RIGHT;

const uint32_t       *row_pins          = NULL;
const uint32_t       *col_pins          = NULL;

static uint8_t enc_col;
static uint8_t enc_row;
static pin_t enc_sw;

void matrix_init_kb(void){
    if (is_keyboard_left()) {
        row_pins = row_pins_left;
        col_pins = col_pins_left;
        enc_col = ENCODER_SW_COL;
        enc_row = ENCODER_SW_ROW;
        enc_sw = ENCODER_SW_PIN;

    } else {
        row_pins = row_pins_right;
        col_pins = col_pins_right;
        enc_col = ENCODER_SW_COL_RIGHT;
        enc_row = ENCODER_SW_ROW_RIGHT - (MATRIX_ROWS / 2); // Adjust row index for right half
        enc_sw = ENCODER_SW_PIN_RIGHT;
    }
    gpio_set_pin_input_high(enc_sw);
    matrix_init_user();
}

void matrix_output_unselect_delay(uint8_t line, bool key_pressed) {
    if (key_pressed) {
        rtcnt_t start = chSysGetRealtimeCounterX();
        rtcnt_t end = start + 1000;

        while (chSysIsCounterWithinX(chSysGetRealtimeCounterX(), start, end)) {
            bool cols_high = true;
            for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
                if (gpio_read_pin(col_pins[col_index]) == 0) {
                    cols_high = false;
                    break;
                }
            }
            if (cols_high) {
                break;
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

    rtcnt_t start = chSysGetRealtimeCounterX();
    rtcnt_t end   = start + 1000;
    while (chSysIsCounterWithinX(chSysGetRealtimeCounterX(), start, end)) {
        if (gpio_read_pin(row_pins[current_row]) == 0) {
            break;
        }
    }

    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        uint8_t pin_state = gpio_read_pin(col_pins[col_index]);


        if (col_index == enc_col && current_row == enc_row){
            if (gpio_read_pin(enc_sw) == 0){
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

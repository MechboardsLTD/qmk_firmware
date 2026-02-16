// Copyright 2026 Mechboards
// SPDX-License-Identifier: GPL-2.0-or-later

#include <printf.h>
#include <ctype.h>
#include "transactions.h"
#include "qp_font/pixellari18.qff.h"
#include "qp_font/pixellari24.qff.h"
#include "display_lcd.h"
#include "lib/lib8tion/lib8tion.h"
#include "rgb_matrix.h"
#include "backlight/backlight.h"

#define WPM_CHART_WIDTH 135
#define WPM_CHART_HEIGHT 70

painter_device_t      lcd;
painter_font_handle_t pixellari_18;
painter_font_handle_t pixellari_24;

static hsv_t mb = {129, 189, 181};

int vals = 20;

static deferred_token display_task_token;

#define LCD_HEIGHT 240
#define LCD_WIDTH 135

void drawtext_right_recolor(painter_device_t device, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    qp_drawtext_recolor(lcd, width - qp_textwidth(font, str), y, font, str, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
}

void drawtext_centered_recolor(painter_device_t device, uint16_t x, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    qp_drawtext_recolor(lcd, (x + (width / 2)) - qp_textwidth(font, str) / 2, y, font, str, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
}

void drawtext_centered(painter_device_t device, uint16_t x, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str) {
    drawtext_centered_recolor(device, x, y, width, font, str, 255, 0, 255, 0, 0, 0);
}

void drawtext_layer(uint16_t x, uint16_t y, uint8_t width, const char *str, uint8_t layer) {
    if (layer == get_highest_layer(layer_state)) {
        drawtext_centered_recolor(lcd, x, y, width, pixellari_24, str, 255, 0, 255, mb.h, mb.s, mb.v);
    } else {
        drawtext_centered_recolor(lcd, x, y, width, pixellari_24, str, 255, 0, 255, 0, 0, 0);
    }
}

void clear_display(void) {
    qp_rect(lcd, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);
}

void draw_layers(void) {
    drawtext_centered(lcd, 0, 10, 135, pixellari_24, "LAYER");
    drawtext_layer(0, 45, 32, "1", 0);
    drawtext_layer(34, 45, 32, "2", 1);
    drawtext_layer(66, 45, 32, "3", 2);
    drawtext_layer(98, 45, 32, "4", 3);
}

void draw_os(bool init) {
    if (init) {
        drawtext_centered(lcd, 0, 190, 135, pixellari_24, "OS");
    }
    char *os_name;
    switch (detected_host_os()) {
        case OS_MACOS:
            os_name = "MacOS";
            break;
        case OS_IOS:
            os_name = "Apple";
            break;
        case OS_WINDOWS:
            os_name = "Windows";
            break;
        case OS_LINUX:
            os_name = "Linux";
            break;
        case OS_UNSURE:
            os_name = "Unsure";
            break;
    }
    drawtext_centered_recolor(lcd, 0, 220, 135, pixellari_18, os_name, mb.h, mb.s, mb.v, 0, 0, 0);
}

void draw_wpm_text(void) {
    char buffer[64] = {0};
    snprintf(buffer, sizeof(buffer), "WPM:%d", get_current_wpm());
    qp_rect(lcd, 0, 80, LCD_WIDTH - 1, 80 + pixellari_24->line_height, 0, 0, 0, true);
    drawtext_centered(lcd, 0, 80, 135, pixellari_24, buffer);
}

struct {
    uint8_t start;
    uint8_t values[WPM_CHART_WIDTH];
} wpm_chart;

void wpm_chart_next(void) {
    wpm_chart.start = (wpm_chart.start + 1) % WPM_CHART_WIDTH;
}

void wpm_chart_write_value(uint8_t value) {
    wpm_chart.values[wpm_chart.start] = value;
    uint8_t scaled_value              = scale8(WPM_CHART_HEIGHT, wpm_chart.values[wpm_chart.start]);
    qp_line(lcd, wpm_chart.start, LCD_HEIGHT - 1 - 60, wpm_chart.start, (LCD_HEIGHT - 1 - 60) - WPM_CHART_HEIGHT, 0, 0, 0);
    qp_line(lcd, wpm_chart.start, LCD_HEIGHT - 1 - 60, wpm_chart.start, (LCD_HEIGHT - 1 - 60) - scaled_value, mb.h, mb.s, mb.v);
    wpm_chart_next();
}

void wpm_chart_init(void) {
    memset(wpm_chart.values, 0, sizeof(wpm_chart.values));
    wpm_chart.start = 0;
}

// void draw_wpm_chart(bool init) {
//     if (init) {
//         wpm_chart_init();
//     }
//     qp_rect(lcd, 0, LCD_HEIGHT - WPM_CHART_HEIGHT - 10, LCD_WIDTH - 1, LCD_HEIGHT - 1 - 10, 0, 0, 0, true);
//     for (uint8_t i = 0; i < WPM_CHART_WIDTH; i++) {
//         uint8_t location     = (wpm_chart.start + i) % WPM_CHART_WIDTH;
//         uint8_t scaled_value = scale8(WPM_CHART_HEIGHT, wpm_chart.values[location]);
//         qp_line(lcd, i, LCD_HEIGHT - 1 - 10, i, (LCD_HEIGHT - 1 - 10) - scaled_value, mb.h, mb.s, mb.v);
//     }
// }

void draw_wpm_chart_2(bool init) {
    if (init) {
        wpm_chart_init();
    }
}

void wpm_layer_display_init(void) {
    clear_display();
    draw_layers();
    draw_wpm_text();
    draw_wpm_chart_2(true);
    qp_flush(lcd);
}

void draw_bar(uint8_t value, uint8_t max_value, uint8_t left, uint8_t top, uint8_t max_length, uint8_t height) {
    uint8_t bar_length = (((max_length << 8) / max_value) * value) >> 8;
    qp_rect(lcd, left, top, left + max_length, top + height, 0, 0, 0, true);
    qp_rect(lcd, left, top, left + max_length, top + height, mb.h, mb.s, mb.v, false);
    qp_rect(lcd, left, top, left + bar_length, top + height, mb.h, mb.s, mb.v, true);
}

void draw_rgb_text(bool init) {
    char buffer[64] = {0};
    if (init) {
        snprintf(buffer, sizeof(buffer), "RGB");
        drawtext_centered(lcd, 0, 10, 135, pixellari_24, buffer);

        qp_drawtext_recolor(lcd, 0, 110, pixellari_18, "Hue:", 0, 0, 255, 0, 0, 0);
        qp_drawtext_recolor(lcd, 0, 130, pixellari_18, "Sat:", 0, 0, 255, 0, 0, 0);
        qp_drawtext_recolor(lcd, 0, 150, pixellari_18, "Val:", 0, 0, 255, 0, 0, 0);
    }

    qp_rect(lcd, LCD_WIDTH / 2, 110, LCD_WIDTH - 1, 110 + pixellari_18->line_height, 0, 0, 0, true);
    snprintf(buffer, sizeof(buffer), "%d", rgb_matrix_get_hue());
    drawtext_right_recolor(lcd, 110, LCD_WIDTH, pixellari_18, buffer, mb.h, mb.s, mb.v, 0, 0, 0);
    // snprintf(buffer, sizeof(buffer), "%d",rgb_matrix_get_sat());
    // drawtext_right_recolor(lcd, 130, LCD_WIDTH, pixellari_18, buffer, mb.h, mb.s, mb.v, 0, 0, 0);
    // snprintf(buffer, sizeof(buffer), "%d",rgb_matrix_get_val());
    // drawtext_right_recolor(lcd, 150, LCD_WIDTH, pixellari_18, buffer, mb.h, mb.s, mb.v, 0, 0, 0);

    draw_bar(rgb_matrix_get_sat(), 255, 45, 130, LCD_WIDTH - 1 - 45, pixellari_18->line_height);
    draw_bar(rgb_matrix_get_val(), RGB_MATRIX_MAXIMUM_BRIGHTNESS, 45, 150, LCD_WIDTH - 1 - 45, pixellari_18->line_height);

    char *mode_name = strdup(rgb_matrix_get_mode_name(rgb_matrix_get_mode()));
    if (mode_name != NULL) {
        int     len               = strlen(mode_name);
        int     underscore_loc[2] = {-1, -1};
        uint8_t underscore_pos    = 0;
        bool    capitalize_next   = true;
        for (int i = 0; i < len; i++) {
            if (mode_name[i] == '_') {
                if (underscore_pos < 2) {
                    mode_name[i]                   = '\0';
                    underscore_loc[underscore_pos] = i;
                    underscore_pos++;
                } else {
                    mode_name[i] = ' ';
                }
                capitalize_next = true;
            } else if (capitalize_next) {
                mode_name[i]    = mode_name[i] >= 'a' && mode_name[i] <= 'z' ? mode_name[i] - 'a' + 'A' : mode_name[i];
                capitalize_next = false;
            } else {
                mode_name[i] = mode_name[i] >= 'A' && mode_name[i] <= 'Z' ? mode_name[i] - 'A' + 'a' : mode_name[i];
            }
        }

        qp_rect(lcd, 0, 30, LCD_WIDTH - 1, 80 + pixellari_18->line_height, 0, 0, 0, true);
        drawtext_centered_recolor(lcd, 0, 40, 135, pixellari_18, &mode_name[0], mb.h, mb.s, mb.v, 0, 0, 0);
        if (underscore_loc[0] > 0) {
            drawtext_centered_recolor(lcd, 0, 60, 135, pixellari_18, &mode_name[underscore_loc[0] + 1], mb.h, mb.s, mb.v, 0, 0, 0);
        }
        if (underscore_loc[1] > 0) {
            drawtext_centered_recolor(lcd, 0, 80, 135, pixellari_18, &mode_name[underscore_loc[1] + 1], mb.h, mb.s, mb.v, 0, 0, 0);
        }
        free(mode_name);
    }
}

void draw_bl_text(bool init) {
    char buffer[64] = {0};
    if (init) {
        snprintf(buffer, sizeof(buffer), "LCD");
        drawtext_centered(lcd, 0, 190, 135, pixellari_24, buffer);
        qp_drawtext_recolor(lcd, 0, 220, pixellari_18, "Bri:", 0, 0, 255, 0, 0, 0);
    }

    draw_bar(get_backlight_level(), BACKLIGHT_LEVELS, 45, 220, LCD_WIDTH - 1 - 45, pixellari_18->line_height);
}

void rgb_bl_display_init(void) {
    clear_display();
    draw_rgb_text(true);
    draw_bl_text(true);
    qp_flush(lcd);
}

uint32_t display_task_callback(uint32_t trigger_time, void *cb_arg) {
    display_task_kb();
    return 100;
}

void display_test(void) {
    qp_rect(lcd, 0, 0, 134, 239, 255, 0, 255, true);
    qp_rect(lcd, 64, 0, 134, 239, 255, 0, 0, true);

    // Draw 8px-wide rainbow filled rectangles down the left side of the display
    for (int i = 0; i < 239; i += 8) {
        qp_rect(lcd, 0, i, 7, i + 7, i, 255, 255, true);
        qp_rect(lcd, 127, i, 134, i + 7, i, 255, 255, true);
    }
    qp_flush(lcd);
}

__attribute__((weak)) bool display_init_user(void) {
    return true;
}

void display_init_kb(void) {
    // Initialise the LCD
    lcd = qp_st7789_make_spi_device(LCD_WIDTH, LCD_HEIGHT, VIK_CS, VIK_GPIO1, VIK_GPIO2, 4, 3);
    qp_init(lcd, QP_ROTATION_0);
    qp_set_viewport_offsets(lcd, 52, 40);
    // Load fonts
    pixellari_18 = qp_load_font_mem(font_pixellari18);
    pixellari_24 = qp_load_font_mem(font_pixellari24);
    if (!display_init_user()) {
        return;
    }

    if (is_keyboard_left()) {
        wpm_layer_display_init();
        draw_os(true);

    } else {
        rgb_bl_display_init();
    }

    display_task_token = defer_exec(2000, display_task_callback, NULL);
}

__attribute__((weak)) bool display_task_user(void) {
    return true;
}

void display_task_kb(void) {
    if (!display_task_user()) {
        return;
    }

    if (is_keyboard_master()) {
        static uint8_t last_bl = 255;

        if (last_input_activity_elapsed() > QUANTUM_PAINTER_DISPLAY_TIMEOUT) {
            if (last_bl == 255) {
                last_bl = get_backlight_level();
            }
            backlight_level_noeeprom(0);
        } else {
            if (last_bl != 255) {
                backlight_level_noeeprom(last_bl);
            }
            last_bl = 255;
        }
    }

    static uint32_t timer = 0;

    if (is_keyboard_left()) {
        if (timer_elapsed(timer) > 100) {
            static uint32_t lastwpm = 0;
            static uint32_t currwpm = 0;

            timer   = timer_read();
            lastwpm = currwpm;
            currwpm = get_current_wpm();

            if (lastwpm != currwpm) {
                draw_wpm_text();
            }
            wpm_chart_write_value(currwpm);
            draw_wpm_chart_2(false);
        }

        static layer_state_t currlay = 0;
        static layer_state_t lastlay = 0;

        currlay = get_highest_layer(layer_state);
        if (currlay != lastlay) {
            draw_layers();
        }

        static os_variant_t last_os = OS_UNSURE;

        if (detected_host_os() != last_os) {
            draw_os(false);
        }

    } else {
        static uint64_t last_rgb = 0;
        if (rgb_matrix_config.raw != last_rgb) {
            last_rgb = rgb_matrix_config.raw;
            draw_rgb_text(false);
        }
        static uint8_t last_bl = 0;
        if (get_backlight_level() != last_bl) {
            last_bl = get_backlight_level();
            draw_bl_text(false);
        }
    }
}

// static uint8_t last_backlight = 255;

// void suspend_power_down_kb(void) {
//     if (last_backlight == 255) {
//         last_backlight = get_backlight_level();
//     }
//     backlight_set(0);
//     suspend_power_down_user();
// }

// void suspend_wakeup_init_kb(void) {
//     if (last_backlight != 255) {
//         backlight_set(last_backlight);
//     }
//     last_backlight = 255;
//     suspend_wakeup_init_user();
// }

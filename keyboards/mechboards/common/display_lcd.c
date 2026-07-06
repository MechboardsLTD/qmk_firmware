// Copyright 2026 Mechboards
// SPDX-License-Identifier: GPL-2.0-or-later

#include <printf.h>
#include <ctype.h>
#include "qp.h"
#include "qp_font/pixellari18.qff.h"
#include "qp_font/pixellari24.qff.h"
#include "display_lcd.h"
#include "lib/lib8tion/lib8tion.h"
#include "rgb_matrix.h"
#include "backlight/backlight.h"

#define WPM_CHART_WIDTH 135
#define WPM_CHART_HEIGHT 70

#define LCD_HEIGHT 240
#define LCD_WIDTH 135

#ifndef LCD_OFFSET_X
#    define LCD_OFFSET_X 52
#endif
#ifndef LCD_OFFSET_Y
#    define LCD_OFFSET_Y 40
#endif

static uint8_t surface_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(LCD_WIDTH, LCD_HEIGHT, 16)];

painter_device_t      lcd;
painter_device_t      surface __attribute__((used));
painter_font_handle_t pixellari_18;
painter_font_handle_t pixellari_24;

static hsv_t ui_hsv = {129, 189, 181};
static hsv_t last_hsv = {0, 0, 255};

static deferred_token display_task_token __attribute__((used));



void drawtext_right_recolor(painter_device_t device, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    qp_drawtext_recolor(surface, width - qp_textwidth(font, str), y, font, str, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
}

void drawtext_centered_recolor(painter_device_t device, uint16_t x, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    qp_drawtext_recolor(surface, (x + (width / 2)) - qp_textwidth(font, str) / 2, y, font, str, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
}

void drawtext_centered(painter_device_t device, uint16_t x, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str) {
    drawtext_centered_recolor(device, x, y, width, font, str, 255, 0, 255, 0, 0, 0);
}

void drawtext_layer(uint16_t x, uint16_t y, uint8_t width, const char *str, uint8_t layer) {
    if (layer == get_highest_layer(layer_state)) {
        drawtext_centered_recolor(surface, x, y, width, pixellari_24, str, 255, 0, 255, ui_hsv.h, ui_hsv.s, ui_hsv.v);
    } else {
        drawtext_centered_recolor(surface, x, y, width, pixellari_24, str, 255, 0, 255, 0, 0, 0);
    }
}

void clear_display(void) {
    qp_rect(surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);
}

void draw_layers(void) {
    drawtext_centered(surface, 0, 10, 135, pixellari_24, "LAYER");
    drawtext_layer(0, 45, 32, "1", 0);
    drawtext_layer(34, 45, 32, "2", 1);
    drawtext_layer(66, 45, 32, "3", 2);
    drawtext_layer(98, 45, 32, "4", 3);
}

void draw_os(bool init) {
    if (init) {
        drawtext_centered(surface, 0, 190, 135, pixellari_24, "OS");
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
        default:
            os_name = "Unsure";
            break;
    }
    qp_rect(surface, 0, 220, LCD_WIDTH-1, 220+pixellari_18->line_height, 0, 0, 0, true);
    drawtext_centered_recolor(surface, 0, 220, 135, pixellari_18, os_name, ui_hsv.h, ui_hsv.s, ui_hsv.v, 0, 0, 0);
}

void draw_wpm_text(void) {
    char buffer[64] = {0};
    snprintf(buffer, sizeof(buffer), "WPM:%d", get_current_wpm());
    qp_rect(surface, 0, 80, LCD_WIDTH - 1, 80 + pixellari_24->line_height, 0, 0, 0, true);
    drawtext_centered(surface, 0, 80, 135, pixellari_24, buffer);
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
    qp_line(surface, wpm_chart.start, LCD_HEIGHT - 1 - 60, wpm_chart.start, (LCD_HEIGHT - 1 - 60) - WPM_CHART_HEIGHT, 0, 0, 0);
    qp_line(surface, wpm_chart.start, LCD_HEIGHT - 1 - 60, wpm_chart.start, (LCD_HEIGHT - 1 - 60) - scaled_value, ui_hsv.h, ui_hsv.s, ui_hsv.v);
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
//     qp_rect(surface, 0, LCD_HEIGHT - WPM_CHART_HEIGHT - 10, LCD_WIDTH - 1, LCD_HEIGHT - 1 - 10, 0, 0, 0, true);
//     for (uint8_t i = 0; i < WPM_CHART_WIDTH; i++) {
//         uint8_t location     = (wpm_chart.start + i) % WPM_CHART_WIDTH;
//         uint8_t scaled_value = scale8(WPM_CHART_HEIGHT, wpm_chart.values[location]);
//         qp_line(surface, i, LCD_HEIGHT - 1 - 10, i, (LCD_HEIGHT - 1 - 10) - scaled_value, ui_hsv.h, ui_hsv.s, ui_hsv.v);
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
}

void draw_bar(uint8_t value, uint8_t max_value, uint8_t left, uint8_t top, uint8_t max_length, uint8_t height) {
    uint8_t bar_length = (((max_length << 8) / max_value) * value) >> 8;
    qp_rect(surface, left, top, left + max_length, top + height, 0, 0, 0, true);
    qp_rect(surface, left, top, left + max_length, top + height, ui_hsv.h, ui_hsv.s, ui_hsv.v, false);
    qp_rect(surface, left, top, left + bar_length, top + height, ui_hsv.h, ui_hsv.s, ui_hsv.v, true);
}

void draw_rgb_text(bool init) {
    char buffer[64] = {0};
    if (init) {
        snprintf(buffer, sizeof(buffer), "RGB");
        drawtext_centered(surface, 0, 10, 135, pixellari_24, buffer);

        qp_drawtext_recolor(surface, 0, 110, pixellari_18, "Hue:", 0, 0, 255, 0, 0, 0);
        qp_drawtext_recolor(surface, 0, 130, pixellari_18, "Sat:", 0, 0, 255, 0, 0, 0);
        qp_drawtext_recolor(surface, 0, 150, pixellari_18, "Val:", 0, 0, 255, 0, 0, 0);
    }

    qp_rect(surface, LCD_WIDTH / 2, 110, LCD_WIDTH - 1, 110 + pixellari_18->line_height, 0, 0, 0, true);
    snprintf(buffer, sizeof(buffer), "%d", rgb_matrix_get_hue());
    drawtext_right_recolor(surface, 110, LCD_WIDTH, pixellari_18, buffer, ui_hsv.h, ui_hsv.s, ui_hsv.v, 0, 0, 0);
    // snprintf(buffer, sizeof(buffer), "%d",rgb_matrix_get_sat());
    // drawtext_right_recolor(surface, 130, LCD_WIDTH, pixellari_18, buffer, ui_hsv.h, ui_hsv.s, ui_hsv.v, 0, 0, 0);
    // snprintf(buffer, sizeof(buffer), "%d",rgb_matrix_get_val());
    // drawtext_right_recolor(surface, 150, LCD_WIDTH, pixellari_18, buffer, ui_hsv.h, ui_hsv.s, ui_hsv.v, 0, 0, 0);

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

        qp_rect(surface, 0, 30, LCD_WIDTH - 1, 80 + pixellari_18->line_height, 0, 0, 0, true);
        drawtext_centered_recolor(surface, 0, 40, 135, pixellari_18, &mode_name[0], ui_hsv.h, ui_hsv.s, ui_hsv.v, 0, 0, 0);
        if (underscore_loc[0] > 0) {
            drawtext_centered_recolor(surface, 0, 60, 135, pixellari_18, &mode_name[underscore_loc[0] + 1], ui_hsv.h, ui_hsv.s, ui_hsv.v, 0, 0, 0);
        }
        if (underscore_loc[1] > 0) {
            drawtext_centered_recolor(surface, 0, 80, 135, pixellari_18, &mode_name[underscore_loc[1] + 1], ui_hsv.h, ui_hsv.s, ui_hsv.v, 0, 0, 0);
        }
        free(mode_name);
    }
}

void draw_bl_text(bool init) {
    char buffer[64] = {0};
    if (init) {
        snprintf(buffer, sizeof(buffer), "LCD");
        drawtext_centered(surface, 0, 190, 135, pixellari_24, buffer);
        qp_drawtext_recolor(surface, 0, 220, pixellari_18, "Bri:", 0, 0, 255, 0, 0, 0);
    }

    draw_bar(get_backlight_level(), BACKLIGHT_LEVELS, 45, 220, LCD_WIDTH - 1 - 45, pixellari_18->line_height);
}

void rgb_bl_display_init(void) {
    clear_display();
    draw_rgb_text(true);
    draw_bl_text(true);
}

uint32_t display_task_callback(uint32_t trigger_time, void *cb_arg) {
    display_task_kb();
    return 100;
}

void display_test(void) {
    qp_rect(surface, 0, 0, 134, 239, 255, 0, 255, true);
    qp_rect(surface, 64, 0, 134, 239, 255, 0, 0, true);

    // Draw 8px-wide rainbow filled rectangles down the left side of the display
    for (int i = 0; i < 239; i += 8) {
        qp_rect(surface, 0, i, 7, i + 7, i, 255, 255, true);
        qp_rect(surface, 127, i, 134, i + 7, i, 255, 255, true);
    }
}

__attribute__((weak)) bool display_init_user(void) {
    return true;
}

void display_ui_init(void) {
    if (is_keyboard_left()) {
        wpm_layer_display_init();
        draw_os(true);
    } else {
        rgb_bl_display_init();
    }
}

__attribute__((weak)) painter_rotation_t get_display_rotation(void) {
    return QP_ROTATION_0;
}

void display_init_kb(void) {
    // Initialise the surface
    lcd = qp_st7789_make_spi_device(LCD_WIDTH, LCD_HEIGHT, VIK_CS, VIK_GPIO1, VIK_GPIO2, 4, 3);
    qp_init(lcd, get_display_rotation());
    qp_set_viewport_offsets(surface, LCD_OFFSET_X, LCD_OFFSET_Y);
    surface = qp_make_rgb565_surface(LCD_WIDTH, LCD_HEIGHT, surface_buffer);
    qp_init(surface, QP_ROTATION_0);
    // Load fonts
    pixellari_18 = qp_load_font_mem(font_pixellari18);
    pixellari_24 = qp_load_font_mem(font_pixellari24);
    if (!display_init_user()) {
        return;
    }

    last_hsv.h = rgb_matrix_get_hue();
    last_hsv.s = rgb_matrix_get_sat();

    ui_hsv = last_hsv;

    display_ui_init();

    qp_surface_draw(surface, lcd, LCD_OFFSET_X, LCD_OFFSET_Y, false);

    display_task_token = defer_exec(2000, display_task_callback, NULL);
}

__attribute__((weak)) bool display_task_user(void) {
    return true;
}

void display_task_kb(void) {
    if (!display_task_user()) {
        return;
    }

    if (last_hsv.h != rgb_matrix_get_hue() || last_hsv.s != rgb_matrix_get_sat()) {
        last_hsv.h = rgb_matrix_get_hue();
        last_hsv.s = rgb_matrix_get_sat();
        ui_hsv      = last_hsv;
        display_ui_init();
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
            lastlay = currlay;
            draw_layers();
        }

        static os_variant_t last_os = OS_UNSURE;

        if (detected_host_os() != last_os) {
            draw_os(false);
            last_os = detected_host_os();
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

    qp_surface_draw(surface, lcd, 52, 40, false);
}

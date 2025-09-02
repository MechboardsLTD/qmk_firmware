#include "screens.h"
#include "printf.h"
#include "transactions.h"
#include "lib/lib8tion/lib8tion.h"

extern painter_device_t      lcd;
extern painter_font_handle_t pixellari_14;
extern painter_font_handle_t pixellari_18;
extern painter_font_handle_t pixellari_24;

static hsv_t mb = {129, 189, 181};

int vals = 20;

static uint8_t bar_start_x;
static uint8_t bar_length;
static uint8_t bar_height;

void drawtext_centered_recolor(painter_device_t device, uint16_t x, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    qp_drawtext_recolor(lcd, (x + (width / 2)) - qp_textwidth(font, str) / 2, y, font, str, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
}

void drawtext_centered(painter_device_t device, uint16_t x, uint16_t y, uint8_t width, painter_font_handle_t font, const char *str) {
    drawtext_centered_recolor(device, x, y, width, font, str, 255, 0, 255, 0, 0, 0);
}

void drawtext_layer(uint16_t x, uint16_t y, uint8_t width, const char *str, uint8_t layer) {
    if (layer == get_highest_layer(layer_state)) {
        drawtext_centered_recolor(lcd, x, y, width, pixellari_18, str, 255, 0, 255, mb.h, mb.s, mb.v);
    } else {
        drawtext_centered_recolor(lcd, x, y, width, pixellari_18, str, 255, 0, 255, 0, 0, 0);
    }
}

void clear_display(void) {
    qp_rect(lcd, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);
}

void draw_layers(void) {
    drawtext_centered(lcd, 0, 10, 80, pixellari_18, "LAYER");
    drawtext_layer(0, 35, 20, "1", 0);
    drawtext_layer(20, 35, 20, "2", 1);
    drawtext_layer(40, 35, 20, "3", 2);
    drawtext_layer(60, 35, 20, "4", 3);
}

void draw_wpm_text(void) {
    char buffer[64] = {0};
    snprintf(buffer, sizeof(buffer), "WPM:%d", get_current_wpm());
    qp_rect(lcd, 0, 60, LCD_WIDTH - 1, 60 + pixellari_18->line_height, 0, 0, 0, true);
    drawtext_centered(lcd, 0, 60, 80, pixellari_18, buffer);
}

#define WPM_CHART_WIDTH 80
#define WPM_CHART_HEIGHT 40
struct {
    uint8_t start;
    uint8_t values[WPM_CHART_WIDTH];
} wpm_chart;

void wpm_chart_next(void) {
    wpm_chart.start = (wpm_chart.start + 1) % WPM_CHART_WIDTH;
}

void wpm_chart_write_value(uint8_t value) {
    wpm_chart.values[wpm_chart.start] = value;
    wpm_chart_next();
}

void wpm_chart_init(void) {
    memset(wpm_chart.values, 0, sizeof(wpm_chart.values));
    wpm_chart.start = 0;
}

void draw_wpm_chart(bool init) {
    if (init) {
        wpm_chart_init();
    }
    qp_rect(lcd, 0, LCD_HEIGHT - WPM_CHART_HEIGHT - 10, LCD_WIDTH - 1, LCD_HEIGHT - 1 - 10, 0, 0, 0, true);
    for (uint8_t i = 0; i < WPM_CHART_WIDTH; i++) {
        uint8_t location     = (wpm_chart.start + i) % WPM_CHART_WIDTH;
        uint8_t scaled_value = scale8(WPM_CHART_HEIGHT, wpm_chart.values[location]);
        qp_line(lcd, i, LCD_HEIGHT - 1 - 10, i, (LCD_HEIGHT - 1 - 10) - scaled_value, mb.h, mb.s, mb.v);
    }
}

void draw_clock(const char *str) {
    char hour[] = "SO";
    char min[]  = "ME";
    char ind[]  = "TM";

    memcpy(hour, str, 2);
    memcpy(min, &str[3], 2);
    memcpy(ind, &str[6], 2);

    drawtext_centered(lcd, 0, 35, 80, pixellari_24, hour);
    drawtext_centered(lcd, 0, 65, 80, pixellari_24, min);
    drawtext_centered(lcd, 0, 95, 80, pixellari_24, ind);
}

void clock_display_init(void) {
    clear_display();
    draw_clock("00\n00\npm");
}

void wpm_layer_display_init(void) {
    clear_display();
    draw_layers();
    draw_wpm_text();
    draw_wpm_chart(true);
    qp_flush(lcd);
}

void draw_pc_stats_text(void) {
    qp_drawtext(lcd, 0, 0, pixellari_14, "CPU");
    qp_drawtext(lcd, 0, pixellari_14->line_height + 1, pixellari_14, "GPU");
    qp_drawtext(lcd, 0, (pixellari_14->line_height + 1) * 2, pixellari_14, "RAM");
}

void draw_bar(uint8_t percent, uint8_t left, uint8_t top, uint8_t max_length, uint8_t height) {
    uint8_t bar_length = (((max_length << 7) / 100) * percent) >> 7;
    qp_rect(lcd, left, top, left + max_length, top + height, 0, 0, 0, true);
    qp_rect(lcd, left, top, left + max_length, top + height, mb.h, mb.s, mb.v, false);
    qp_rect(lcd, left, top, left + bar_length, top + height, mb.h, mb.s, mb.v, true);
}

void draw_bar_cpu(uint8_t percent) {
    draw_bar(percent, bar_start_x, 0, bar_length, bar_height);
}
void draw_bar_gpu(uint8_t percent) {
    draw_bar(percent, bar_start_x, pixellari_14->line_height + 1, bar_length, bar_height);
}
void draw_bar_ram(uint8_t percent) {
    draw_bar(percent, bar_start_x, (pixellari_14->line_height + 1) * 2, bar_length, bar_height);
}

void draw_pc_stats_graphs(uint8_t cpu, uint8_t gpu, uint8_t ram) {
    draw_bar_cpu(cpu);
    draw_bar_gpu(gpu);
    draw_bar_ram(ram);
}

void draw_layers_tight(void) {
    drawtext_centered(lcd, 0, 48, 80, pixellari_18, "LAYER");
    drawtext_layer(0, 67, 20, "1", 0);
    drawtext_layer(20, 67, 20, "2", 1);
    drawtext_layer(40, 67, 20, "3", 2);
    drawtext_layer(60, 67, 20, "4", 3);
}

void draw_wpm_text_tight(void) {
    char buffer[64] = {0};
    snprintf(buffer, sizeof(buffer), "WPM:%d", get_current_wpm());
    qp_rect(lcd, 0, 87, LCD_WIDTH - 1, 87 + pixellari_18->line_height, 0, 0, 0, true);
    drawtext_centered(lcd, 0, 87, 80, pixellari_18, buffer);
}

void pc_layer_wpm_display_init(void) {
    clear_display();
    bar_start_x = qp_textwidth(pixellari_14, "RAM") + 1;
    bar_length  = LCD_WIDTH - 1 - bar_start_x;
    bar_height  = pixellari_14->line_height - 2;

    draw_pc_stats_text();
    draw_pc_stats_graphs(0, 0, 0);
    draw_layers_tight();
    draw_wpm_text_tight();
    qp_flush(lcd);
}

void draw_clock_title(const char *str) {
    drawtext_centered(lcd, 0, 0, LCD_WIDTH - 1, pixellari_18, str);
}

void draw_bar_progress(uint8_t percent) {
    draw_bar(percent, 0, 150, LCD_WIDTH - 1, 5);
}

void spotify_display_init(void) {
    clear_display();
    draw_clock_title("00:00am");
    draw_bar_progress(0);

    qp_rect(lcd, 8, 35, LCD_WIDTH - 1 - 8, 35 + 64, mb.h, mb.s, mb.v, false);

    drawtext_centered(lcd, 0, 120, LCD_WIDTH - 1, pixellari_18, "Now playing");
}

void gif_display_init(void) {
    clear_display();
    drawtext_centered(lcd, 0, 60, LCD_WIDTH - 1, pixellari_18, "Not");
    drawtext_centered(lcd, 0, 80, LCD_WIDTH - 1, pixellari_18, "supported");
    // gif = lv_gif_create(lv_scr_act());
    // lv_gif_set_src(gif, &ezgif);
    // lv_obj_align(gif, LV_ALIGN_TOP_MID, 0, 30);
}
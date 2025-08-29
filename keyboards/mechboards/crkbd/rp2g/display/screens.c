#include "screens.h"
#include "printf.h"
#include "transactions.h"

int                vals       = 20;

void spotify_display_init(void) {
    // label_time = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_time, "00:00");
    // lv_obj_align(label_time, LV_ALIGN_TOP_MID, 0, 4);

    // album_art = lv_img_create(lv_scr_act());
    // lv_img_set_src(album_art, &mb);
    // lv_obj_align(album_art, LV_ALIGN_TOP_MID, 0, 30);

    // label_track = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_track, "Now Playing");
    // lv_obj_align(label_track, LV_ALIGN_CENTER, 0, 35);
    // lv_obj_set_width(label_track, 74);
    // lv_obj_set_style_text_color(label_track, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    // // lv_obj_set_style_text_font(label_track, LV_FONT_MONTSERRAT_28, LV_PART_MAIN);
    // lv_label_set_long_mode(label_track, LV_LABEL_LONG_SCROLL_CIRCULAR);

    // progress = lv_slider_create(lv_scr_act());
    // lv_obj_set_size(progress, 70, 4);
    // lv_obj_align(progress, LV_ALIGN_CENTER, 0, 60);
    // lv_slider_set_range(progress, 0, 100);
    // lv_obj_add_style(progress, &style_main, LV_PART_MAIN);
    // lv_obj_add_style(progress, &style_indicator, LV_PART_INDICATOR);
    // lv_obj_add_style(progress, &style_knob, LV_PART_KNOB);
}
void gif_display_init(void) {
    // gif = lv_gif_create(lv_scr_act());
    // lv_gif_set_src(gif, &ezgif);
    // lv_obj_align(gif, LV_ALIGN_TOP_MID, 0, 30);
}

void pc_layer_wpm_display_init(void) {
    // label_cpu = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_cpu, "CPU");
    // lv_obj_align(label_cpu, LV_ALIGN_TOP_LEFT, 1, 1);
    // lv_obj_set_style_text_font(label_cpu, &pixellari_14, LV_PART_MAIN);

    // bar_cpu = lv_bar_create(lv_scr_act());
    // lv_obj_set_size(bar_cpu, 48, 13);
    // lv_obj_align(bar_cpu, LV_ALIGN_TOP_LEFT, 30, 1);
    // lv_obj_add_style(bar_cpu, &bar_style_bg, 0);
    // lv_obj_add_style(bar_cpu, &bar_style_indic, LV_PART_INDICATOR);
    // lv_bar_set_range(bar_cpu, 0, 100);
    // lv_bar_set_value(bar_cpu, 75, LV_ANIM_OFF);

    // label_gpu = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_gpu, "GPU");
    // lv_obj_align(label_gpu, LV_ALIGN_TOP_LEFT, 1, 16);
    // lv_obj_set_style_text_font(label_gpu, &pixellari_14, LV_PART_MAIN);

    // bar_gpu = lv_bar_create(lv_scr_act());
    // lv_obj_set_size(bar_gpu, 48, 13);
    // lv_obj_align(bar_gpu, LV_ALIGN_TOP_LEFT, 30, 16);
    // lv_obj_add_style(bar_gpu, &bar_style_bg, 0);
    // lv_obj_add_style(bar_gpu, &bar_style_indic, LV_PART_INDICATOR);
    // lv_bar_set_range(bar_gpu, 0, 100);
    // lv_bar_set_value(bar_gpu, 50, LV_ANIM_OFF);

    // label_ram = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_ram, "RAM");
    // lv_obj_align(label_ram, LV_ALIGN_TOP_LEFT, 1, 31);
    // lv_obj_set_style_text_font(label_ram, &pixellari_14, LV_PART_MAIN);

    // bar_ram = lv_bar_create(lv_scr_act());
    // lv_obj_set_size(bar_ram, 48, 13);
    // lv_obj_align(bar_ram, LV_ALIGN_TOP_LEFT, 30, 31);
    // lv_obj_add_style(bar_ram, &bar_style_bg, 0);
    // lv_obj_add_style(bar_ram, &bar_style_indic, LV_PART_INDICATOR);
    // lv_bar_set_range(bar_ram, 0, 100);
    // lv_bar_set_value(bar_ram, 95, LV_ANIM_OFF);

    // // Layer Indication
    // label_layer = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_layer, "LAYER");
    // lv_obj_align(label_layer, LV_ALIGN_CENTER, 0, -24);

    // layer_btn_matrix = lv_btnmatrix_create(lv_scr_act());
    // lv_obj_set_size(layer_btn_matrix, 80, 24);
    // lv_obj_align(layer_btn_matrix, LV_ALIGN_CENTER, 0, -2);
    // lv_btnmatrix_set_map(layer_btn_matrix, btnm_map);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 2, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 3, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_obj_add_style(layer_btn_matrix, &style_btn_matrix, LV_PART_MAIN);
    // lv_obj_add_style(layer_btn_matrix, &style_btn, LV_PART_ITEMS);
    // lv_obj_add_style(layer_btn_matrix, &style_btn_checked, LV_PART_ITEMS | LV_STATE_CHECKED);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 0, LV_BTNMATRIX_CTRL_CHECKED);
    // lv_btnmatrix_set_one_checked(layer_btn_matrix, true);

    // /*WPM chart and Label*/
    // label_wpm = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_wpm, "WPM:0");
    // lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    // lv_obj_align(label_wpm, LV_ALIGN_CENTER, 0, 20);

    // chart = lv_chart_create(lv_scr_act());
    // lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    // lv_chart_set_point_count(chart, vals);
    // lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 150);
    // lv_obj_set_style_pad_column(chart, 0.1, 0);
    // lv_obj_set_size(chart, 80, 40);
    // lv_obj_align(chart, LV_ALIGN_CENTER, 0, 50);
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 0, 0, 0, 0, 0, 0);
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 0, 0, 0, 0);

    // ser = lv_chart_add_series(chart, lv_color_hex(0x30b1b6), LV_CHART_AXIS_PRIMARY_Y);
    // lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    // lv_obj_set_style_size(chart, 3, LV_PART_ITEMS);

    // lv_obj_add_style(chart, &style_chart, LV_PART_MAIN);
}

void clock_display_init(void) {
    // label_time = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_time, "00\n00");
    // lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_style_text_font(label_time, &pixellari_24, LV_PART_MAIN);
}

void wpm_layer_display_init(void) {
    // // Layer Indication
    // label_layer = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_layer, "LAYER");
    // lv_obj_align(label_layer, LV_ALIGN_CENTER, 0, -55);

    // layer_btn_matrix = lv_btnmatrix_create(lv_scr_act());
    // lv_obj_set_size(layer_btn_matrix, 80, 24);
    // lv_obj_align(layer_btn_matrix, LV_ALIGN_CENTER, 0, -25);
    // lv_btnmatrix_set_map(layer_btn_matrix, btnm_map);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 2, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 3, LV_BTNMATRIX_CTRL_CHECKABLE);
    // lv_obj_add_style(layer_btn_matrix, &style_btn_matrix, LV_PART_MAIN);
    // lv_obj_add_style(layer_btn_matrix, &style_btn, LV_PART_ITEMS);
    // lv_obj_add_style(layer_btn_matrix, &style_btn_checked, LV_PART_ITEMS | LV_STATE_CHECKED);
    // lv_btnmatrix_set_btn_ctrl(layer_btn_matrix, 0, LV_BTNMATRIX_CTRL_CHECKED);
    // lv_btnmatrix_set_one_checked(layer_btn_matrix, true);

    // /*WPM chart and Label*/
    // label_wpm = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_wpm, "WPM:0");
    // lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    // lv_obj_align(label_wpm, LV_ALIGN_CENTER, 0, 5);

    // chart = lv_chart_create(lv_scr_act());
    // lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    // lv_chart_set_point_count(chart, vals);
    // lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 150);
    // lv_obj_set_style_pad_column(chart, 0.1, 0);
    // lv_obj_set_size(chart, 80, 40);
    // lv_obj_align(chart, LV_ALIGN_CENTER, 0, 40);
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 0, 0, 0, 0, 0, 0);
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 0, 0, 0, 0);

    // ser = lv_chart_add_series(chart, lv_color_hex(0x30b1b6), LV_CHART_AXIS_PRIMARY_Y);
    // lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    // lv_obj_set_style_size(chart, 3, LV_PART_ITEMS);

    // lv_obj_add_style(chart, &style_chart, LV_PART_MAIN);
}

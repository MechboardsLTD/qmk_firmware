#include "qp.h"

void spotify_display_init(void);
void gif_display_init(void);
void pc_layer_wpm_display_init(void);
void clock_display_init(void);
void wpm_layer_display_init(void);

void wpm_chart_write_value(uint8_t value);
void draw_wpm_chart(bool init);
void draw_wpm_text(void);
void draw_wpm_text_tight(void);

void draw_layers(void);
void draw_layers_tight(void);
void draw_clock(const char *str);
void draw_bar_cpu(uint8_t percent);
void draw_bar_gpu(uint8_t percent);
void draw_bar_ram(uint8_t percent);

int vals;

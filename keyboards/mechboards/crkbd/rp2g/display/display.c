
#include "display.h"
#include "qp.h"
#include "qp_comms.h"
#include "qp_st77xx_opcodes.h"
#include "printf.h"
#include "transactions.h"
#include "screens.h"
#include "display/gfx/pixellari14.qff.h"
#include "display/gfx/pixellari18.qff.h"
#include "display/gfx/pixellari24.qff.h"

painter_device_t lcd;
painter_font_handle_t pixellari_14;
painter_font_handle_t pixellari_18;
painter_font_handle_t pixellari_24;

bool    master;
int     currwpm      = 0;
int     lastwpm      = 0;
int     timer        = 0;
int     count        = 0;
int     currlay      = 0;
int     lastlay      = 0;
uint8_t currScreen   = 0xFF;

void display_housekeeping_task(void) {
    if (timer_elapsed(timer) > 1000 && (currScreen == 0x00 || currScreen == 0x01)) {
        timer        = timer_read();
        lastwpm      = currwpm;
        currwpm      = get_current_wpm();

        if (lastwpm != currwpm) {
            if (currScreen) {
                draw_wpm_text_tight();
            } else {
                draw_layers_tight();
            }
        }
        wpm_chart_write_value(currwpm);
        draw_wpm_chart(false);
    }

    if (currScreen == 0x00 || currScreen == 0x01) {
        currlay = get_highest_layer(layer_state);
        if (currlay != lastlay) {
            lastlay = currlay;
            switch (currlay) {
                case 0:
                case 1:
                case 2:
                case 3:
                    if (currScreen) {
                        draw_layers_tight();
                    } else {
                        draw_layers();
                    }
                    break;
            }
        }
    }
}

// Read string from data to string_data to print on the display
void read_string(uint8_t *data, char *string_data, uint8_t length) {
    // uint8_t data_length = data[2];
    memcpy(string_data, data + 4, length);
    string_data[length] = '\0';
}

void processes_command(uint8_t *data, uint8_t length) {
    uint8_t command = data[2];
    char    string[length - 2];
    uprintf("Command: %d\n", command);
    // uprintf("data[4] %u", data[4]);
    switch (command) {
        case _SCREEN:
            draw_screen(data[4]);
            break;
        case _TIME:
            read_string(data, string, length - 2);
            uprintf("Time: %s\n", string);
            draw_clock(string);
            break;
        case _RAM:
            draw_bar_ram(data[4]);
            qp_flush(lcd);
            uprintf("RAM: %d\n", data[4]);
            break;
        case _CPU:
            draw_bar_cpu(data[4]);
            qp_flush(lcd);

            uprintf("CPU: %d\n", data[4]);
            break;
        case _GPU:
            draw_bar_gpu(data[4]);
            qp_flush(lcd);

            uprintf("GPU: %d\n", data[4]);
            break;
        case _PROGRESS:
            // lv_slider_set_value(progress, data[4], LV_ANIM_ON);
            uprintf("PROGRESS: %d\n", data[4]);
            break;
        case _NOWPLAYING:
            read_string(data, string, length - 4);
            // lv_label_set_text_fmt(label_track, "%s", string);
            uprintf("Now Playing: %s\n", string);
            break;
        case _IMAGE: {
            uint16_t x = ((uint16_t)data[4] << 8) | data[5];
            uprintf("Image %d %d %hu %d\n", data[4], data[5], x, data[6]);
            // memcpy(&lv_mb_map[x * 25], &data[7], data[6]);
            break;
        }
        case _IMG_FS: {
            uprintf("Image FS\n");
            uint16_t y = ((uint16_t)data[4] << 8) | data[5];
            uprintf("Image %d %d %hu %d\n", data[4], data[5], y, data[6]);
            // memcpy(&lv_scr[(y * 25)], &data[7], data[6]);
            break;
        }
        case _IMG_GIF: {
            uprintf("Image GIF\n");
            uint16_t z = ((uint16_t)data[4] << 8) | data[5];
            uprintf("GIF %d %d %hu %d\n", data[4], data[5], z, data[6]);
            // memcpy(&ezgif_map[(z * 25)], &data[7], data[6]);
            break;
        }
        case _STATUS:
            uprintf("staus: %d\n", data[4]);
            switch (data[4]) {
                case _IMAGE:
                    uprintf("Image Set\n");
                    // lv_img_set_src(album_art, &mb);
                    break;
                case _IMG_FS:
                    uprintf("Image FS Set\n");
                    // lv_img_set_src(img_scr, &scr);
                    break;
                case _IMG_GIF:
                    if (data[5] == 0x00) {
                        uprintf("Gif Pause\n");
                        // lv_obj_del(gif);
                        draw_screen(0xFF);
                    } else {
                        uprintf("Gif Set %d\n", ((uint16_t)data[6] << 8) | data[7]);
                        // ezgif.data_size = ((uint16_t)data[6] << 8) | data[7];
                        draw_screen(4);
                    }
                    break;
            }
    }
}

// Takes the data from the host and processes it
// This works whether VIA is enabled or just RawHID
// Data is in the format:
// data[0] VIA COMMAND_ID (0x07 - custom set)
// data[1] VIA Channel_ID (0x00)
// data[2] display_data_type
// data[3] master = 0x00, slave = 0x01
// data[4:31] data
#ifdef RAW_ENABLE
#    ifdef VIA_ENABLE
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
#    else
void raw_hid_receive(uint8_t *data, uint8_t length) {
#    endif
    if (data[1] != 0x00) return;
    if (!master) return;
    if (data[3] == 0x00)
        processes_command(data, length);
    else if (data[3] == 0x01)
        transaction_rpc_send(SCR_SLV_SYNC, length, data);
}
#endif

// This function is called when the master sends data to the slave, which is then processed with processes_command
void screen_sync_slave_handler(uint8_t length, const void *in_data, uint8_t out_buflen, void *out_data) {
    uint8_t *data = (uint8_t *)in_data;
    processes_command(data, length);
}

void display_init(void) {
    master = is_keyboard_master();
    transaction_register_rpc(SCR_SLV_SYNC, screen_sync_slave_handler);
    timer = timer_read();
    uprintf("Post Init\n");
    wait_ms(LCD_WAIT_TIME);
    // Initialise the LCD
    lcd = qp_st7735_make_spi_device(LCD_WIDTH, LCD_HEIGHT, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, LCD_SPI_DIVISOR, LCD_SPI_MODE);
    qp_init(lcd, LCD_ROTATION);

    // Load fonts
    pixellari_14 = qp_load_font_mem(font_pixellari14);
    pixellari_18 = qp_load_font_mem(font_pixellari18);
    pixellari_24 = qp_load_font_mem(font_pixellari24);

    // Apply Offset
    qp_set_viewport_offsets(lcd, LCD_OFFSET_X, LCD_OFFSET_Y);

// Invert Colour
#ifdef LCD_INVERT_COLOUR
    qp_comms_start(lcd);
    qp_comms_command(lcd, ST77XX_CMD_INVERT_ON);
    qp_comms_stop(lcd);
#endif
    qp_rect(lcd, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);

    draw_screen(0);
}

void draw_screen(uint8_t screen) {
    switch (screen) {
        case 0x00:
            wpm_layer_display_init();
            break;
        case 0x01:
            pc_layer_wpm_display_init();
            break;
        case 0x02:
            clock_display_init();
            break;
        case 0x03:
            spotify_display_init();
            break;
        case 0x04:
            gif_display_init();
            break;

        default:
            break;
    }
    currScreen = screen;
    // Whole Screen Custom Image Container
    // img_scr = lv_img_create(lv_scr_act());
}

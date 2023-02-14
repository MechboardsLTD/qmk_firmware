// Copyright 2023 Dasky @daskygit
// Copyright 2021 HorrorTroll <https://github.com/HorrorTroll>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#define IDLE_FRAMES 5
#define IDLE_FRAME_DURATION 200     // how long each frame lasts in ms

uint32_t bongo_timer        = 0;
uint8_t  current_idle_frame = 0;
bool toggle_tap_frame = false;

typedef struct {
    const char*    block_map;
    const char*    block_list;
} compressed_oled_frame_t;

static const char PROGMEM idle_map1[] = {    0xc0, 0x7f, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00};

static const char PROGMEM idle_list1[] = {    0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0,
    0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xc0, 0xc0,
    0xc0, 0x80, 0x80, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
    0xf7, 0x31, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40,
    0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe,
    0xff, 0xff, 0xff, 0x3f, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x80, 0xc0, 0xe0, 0x30, 0x38,
    0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27,
    0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f, 0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f,
    0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08,
    0x08, 0x18, 0x10, 0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e,
    0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93,
    0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2,
    0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63,
    0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20,
    0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80};

static const char PROGMEM idle_map2[] = { 0xc0, 0x7f, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00};

static const char PROGMEM idle_list2[] = {0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8,
    0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x01, 0x01,
    0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x08, 0x10, 0x10,
    0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0,
    0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0x9f, 0x9f, 0xff,
    0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x03,
    0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f,
    0x8f, 0x8f, 0x9f, 0x9f, 0x1e, 0x1e, 0x3f, 0x3d, 0x3d, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0xf9,
    0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
    0xf0, 0xc0, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30, 0x60,
    0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
    0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c,
    0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e,
    0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf,
    0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40,
    0x80, 0x80, 0x80, 0x80};

static const char PROGMEM idle_map3[] = {    0xc0, 0x7f, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00};

static const char PROGMEM idle_list3[] = {    0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8,
    0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x01, 0x01, 0x01,
    0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x08, 0x10, 0x10, 0x10,
    0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0,
    0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0x9f, 0x9f, 0xff, 0xff,
    0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0x7e, 0x03, 0x07,
    0x0f, 0x0f, 0x0f, 0x01, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f,
    0x8f, 0x9f, 0x9f, 0x1e, 0x1e, 0x3f, 0x3d, 0x3d, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0xf9, 0xf9,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0,
    0xc0, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30, 0x60, 0x40,
    0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d,
    0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
    0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f,
    0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf,
    0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80,
    0x80, 0x80, 0x80};

static const char PROGMEM idle_map4[] = {    0xc0, 0x7f, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00};

static const char PROGMEM idle_list4[] = {    0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8,
    0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x01, 0x01, 0x01,
    0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x08, 0x10, 0x10, 0x10,
    0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0,
    0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff,
    0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0x3c, 0x1c, 0x03,
    0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f,
    0x8f, 0x8f, 0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc,
    0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
    0xf8, 0xe0, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30, 0x60,
    0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
    0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c,
    0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e,
    0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf,
    0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40,
    0x80, 0x80, 0x80, 0x80};

static const char PROGMEM idle_map5[] = {    0xc0, 0x7f, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00};

    static const char PROGMEM idle_list5[] = {    0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0,
    0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xc0, 0xc0,
    0xc0, 0x80, 0x80, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
    0xf7, 0x31, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40,
    0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0xff,
    0xff, 0xff, 0xff, 0x3f, 0x0f, 0x06, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x80, 0xc0, 0xe0,
    0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f,
    0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f, 0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e,
    0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18,
    0x08, 0x08, 0x08, 0x18, 0x10, 0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69,
    0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d,
    0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36,
    0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f,
    0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20,
    0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80};

static const char PROGMEM tap_map1[] = {    0xc0, 0x7f, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00};

static const char PROGMEM tap_list1[] = {    0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0,
    0xf0, 0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80,
    0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x08,
    0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0xf0,
    0xf8, 0xdc, 0x36, 0x3e, 0xee, 0xfc, 0xe0, 0x9c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf,
    0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff,
    0x3f, 0x80, 0x80, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c,
    0x44, 0xc4, 0xc4, 0x68, 0x78, 0x59, 0x89, 0x91, 0x91, 0xd3, 0xf3, 0xa3, 0x23, 0x27, 0x27,
    0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f, 0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f,
    0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x3f, 0x1f, 0x07, 0x83, 0xc0, 0xe0, 0xe0, 0xc0, 0x80,
    0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30, 0x60, 0x40, 0xc0, 0x86,
    0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x13, 0x12, 0x3c, 0x2c, 0x26, 0x27,
    0x45, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5,
    0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f,
    0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
    0x10, 0x10, 0x11, 0x11, 0x21, 0x23, 0x23, 0x21, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80,
    0x80};

static const char PROGMEM tap_map2[] = {    0xc0, 0x7f, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x1f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00};

static const char PROGMEM tap_list2[] = {    0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0,
    0xf0, 0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80,
    0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x08,
    0x10, 0x10, 0x10, 0x10, 0xa0, 0xa0, 0x20, 0x20, 0x40, 0x46, 0x4f, 0x5f, 0x9f, 0x9c, 0x90,
    0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf,
    0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x7e, 0xbe, 0xbc, 0xbc, 0x7c, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff,
    0x3f, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x07, 0x0f, 0x8f, 0xc7, 0xe3, 0x31, 0x38, 0x2c,
    0x04, 0x64, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27,
    0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f, 0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f,
    0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0x81, 0xfa, 0xff, 0xe6, 0xe7, 0xfd, 0xff, 0xfc, 0xf3,
    0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08,
    0x18, 0x10, 0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13,
    0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3,
    0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62,
    0x34, 0x3c, 0x2c, 0x44, 0xc8, 0xc8, 0xe9, 0xb9, 0x11, 0x11, 0x13, 0x93, 0xe3, 0x63, 0x27,
    0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
    0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80};

static const compressed_oled_frame_t idle[] = {{idle_map1, idle_list1}, {idle_map2, idle_list2}, {idle_map3, idle_list3}, {idle_map4, idle_list4}, {idle_map5, idle_list5}};
static const compressed_oled_frame_t tap[] = {{tap_map1, tap_list1}, {tap_map2, tap_list2}};

static void oled_write_compressed_P(compressed_oled_frame_t frame) {
    uint16_t block_index = 0;
    for (uint16_t i = 0; i < 512; i++) {
        uint8_t bit          = i % 8;
        uint8_t map_index    = i / 8;
        uint8_t _block_map   = (uint8_t)pgm_read_byte_near(frame.block_map + map_index);
        uint8_t nonzero_byte = (_block_map & (1 << bit));
        if (nonzero_byte) {
            const char data = (const char)pgm_read_byte_near(frame.block_list + block_index++);
            oled_write_raw_byte(data, i);
        } else {
            const char data = (const char)0x00;
            oled_write_raw_byte(data, i);
        }
    }
}

void render_bongocat(void) {
    // assumes 1 frame prep stage
    void animation_phase(void) {
        if (last_matrix_activity_elapsed() > IDLE_FRAME_DURATION){
            current_idle_frame = (current_idle_frame+1)%IDLE_FRAMES;
            oled_write_compressed_P(idle[current_idle_frame]);
         } else {
            oled_write_compressed_P(tap[toggle_tap_frame]);
            toggle_tap_frame =  !toggle_tap_frame;
        }
    }
        extern bool some_key_pressed;
        if (some_key_pressed || timer_elapsed32(bongo_timer) > IDLE_FRAME_DURATION) {
            some_key_pressed = false;
            bongo_timer = timer_read32();
            animation_phase();
        }

}

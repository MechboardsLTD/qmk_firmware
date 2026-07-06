// Copyright 2025 Dasky (@daskygit)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "common/display_lcd.h"

void keyboard_post_init_kb(void) {
    display_init_kb();
    keyboard_post_init_user();
}

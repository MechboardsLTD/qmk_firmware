// Copyright 2026 Dasky (@daskygit)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"

#ifdef QUANTUM_PAINTER_ENABLE
#include "common/display_lcd.h"

void keyboard_post_init_kb(void) {
    display_init_kb();
    keyboard_post_init_user();
}

painter_rotation_t get_display_rotation(void) {
    return QP_ROTATION_180;
}
#endif

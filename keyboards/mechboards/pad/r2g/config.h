// Copyright 2026 Dasky (@daskygit)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "keyboards/mechboards/common/rgb_effects.h"


#define RGB_MATRIX_MODE_NAME_ENABLE
#define RGB_MATRIX_CENTER { 32, 32 }

#define QUANTUM_PAINTER_SUPPORTS_256_PALETTE true

#define VIK_GPIO1 GP28
#define VIK_GPIO2 GP29
#define VIK_CS GP11

#define I2C_DRIVER I2CD0
#define I2C1_SDA_PIN GP8
#define I2C1_SCL_PIN GP9

#define SPI_DRIVER SPID1
#define SPI_SCK_PIN GP14
#define SPI_MOSI_PIN GP15
#define SPI_MISO_PIN GP12

#define BACKLIGHT_PWM_DRIVER PWMD4
#define BACKLIGHT_PWM_CHANNEL RP2040_PWM_CHANNEL_B

#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64

#define LCD_OFFSET_X 53

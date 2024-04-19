// Copyright 2023 jack (@waffle87)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// RGB MATRIX
#define RGB_MATRIX_LED_COUNT 54
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_ALPHAS_MODS
#define RGB_DISABLE_WHEN_USB_SUSPENDED
#define RGB_MATRIX_DEFAULT_HUE 0
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_DEFAULT_VAL 255
#define RGB_MATRIX_DEFAULT_SPEED 4

// OPERATIONS
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP22
#define I2C1_SCL_PIN GP23

// AUDIO
#define AUDIO_PIN GP28
#define AUDIO_PWM_DRIVER PWMD6
#define AUDIO_INIT_DELAY
#define AUDIO_PWM_CHANNEL RP2040_PWM_CHANNEL_A
#define STARTUP_SONG SONG(GUITAR_SOUND)

// INPUT
#define ANALOG_JOYSTICK_Y_AXIS_PIN GP26
#define ANALOG_JOYSTICK_X_AXIS_PIN GP27
#define POINTING_DEVICE_INVERT_Y
#define POINTING_DEVICE_INVERT_X
#define SPLIT_LAYER_STATE_ENABLE

// OLED DISPLAY
#define OLED_FADE_OUT
#define OLED_FADE_OUT_INTERVAL 15
#define SPLIT_WPM_ENABLE

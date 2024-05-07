#pragma once

#include "rgb_matrix.h"

#define LILLI_REVEAL_SPEED 250
#define LILLI_LEDS(leds) leds, sizeof(leds) / sizeof(leds[0])
#define LILLI_HSV(hsv) hsv.h, hsv.s, hsv.v

static void lilli_set_hsv_color_multi(const int leds[], int count, uint8_t h, uint8_t s, uint8_t v) {
    HSV hsvcolor = { h, s, v };
    RGB rgbcolor = hsv_to_rgb(hsvcolor);
    for (int i = 0; i < count; i++)
        rgb_matrix_set_color(leds[i], rgbcolor.r, rgbcolor.g, rgbcolor.b);
}

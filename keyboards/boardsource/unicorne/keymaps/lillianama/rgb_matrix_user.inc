
RGB_MATRIX_EFFECT(LILLIAN_KEYMAP_EFFECT)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

const int lili_purple[] = { 6, 24, 25, 26, 33, 51, 53 };
const int lili_green[] = { 13, 40 };
const int lili_red[] = { 14, 41 };
const int lili_white[] = { 7,8,9,10,11,12,15,16,17,18,19,20,21,22,23,34,35,36,37,38,39,42,43,44,45,46,47,48,49,50,52 };

static bool LILLIAN_KEYMAP_EFFECT(effect_params_t* params) {

    int myval = rgb_matrix_get_val();
    HSV lili_purple_hsv = { 190, 255, myval };
    HSV lili_green_hsv = { 85, 255, myval };
    HSV lili_red_hsv = { 0, 255, myval };
    HSV lili_white_hsv = { 0, 0, myval };

    RGB lili_purple_rgb = hsv_to_rgb(lili_purple_hsv);
    RGB lili_green_rgb = hsv_to_rgb(lili_green_hsv);
    RGB lili_red_rgb = hsv_to_rgb(lili_red_hsv);
    RGB lili_white_rgb = hsv_to_rgb(lili_white_hsv);

    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    /* for (uint8_t i = led_min; i < led_max; i++) { } */
    int i;
    for (i=0; i < sizeof(lili_purple) / sizeof(lili_purple[0]); i++)
        rgb_matrix_set_color(lili_purple[i], lili_purple_rgb.r, lili_purple_rgb.g, lili_purple_rgb.b);
    for (i=0; i < sizeof(lili_green) / sizeof(lili_green[0]); i++)
        rgb_matrix_set_color(lili_green[i], lili_green_rgb.r, lili_green_rgb.g, lili_green_rgb.b);
    for (i=0; i < sizeof(lili_red) / sizeof(lili_red[0]); i++)
        rgb_matrix_set_color(lili_red[i], lili_red_rgb.r, lili_red_rgb.g, lili_red_rgb.b);
    for (i=0; i < sizeof(lili_white) / sizeof(lili_white[0]); i++)
        rgb_matrix_set_color(lili_white[i], lili_white_rgb.r, lili_white_rgb.g, lili_white_rgb.b);

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS

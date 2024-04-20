#include QMK_KEYBOARD_H
#include "lib/lib8tion/lib8tion.h"
#include "print.h"

#define LILLI_REVEAL_SPEED 250
#define LILLI_LEDS(leds) leds, sizeof(leds) / sizeof(leds[0])
#define LILLI_HSV(hsv) hsv.h, hsv.s, hsv.v

static void lilli_set_hsv_colors(const int leds[], int count, uint8_t h, uint8_t s, uint8_t v) {
    HSV hsvcolor = { h, s, v };
    RGB rgbcolor = hsv_to_rgb(hsvcolor);
    for (int i = 0; i < count; i++)
        rgb_matrix_set_color(leds[i], rgbcolor.r, rgbcolor.g, rgbcolor.b);
}

#ifdef OLED_ENABLE

#    include "crab.c" //Walking crab animation

#    define ANIM_INVERT false
#    define ANIM_RENDER_WPM true
#    define FAST_TYPE_WPM 45 // Switch to fast animation when over words per minute

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;
    }
    return rotation;
}

bool oled_task_user(void) {
    if (!is_keyboard_master()) {
        oled_render_anim();
    }
    return false;
}

#endif // OLED_ENABLE

#ifdef AUDIO_ENABLE
float function_layer_sound[][2] = SONG(TERMINAL_SOUND);
#endif // AUDIO_ENABLE

/*
 * Lillian's keymap definition with tri-layer function layer
 * and light up key indicators
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_split_3x6_3(
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_ESC,
        KC_LGUI, QK_TRI_LAYER_LOWER, KC_SPC, KC_ENT, QK_TRI_LAYER_UPPER, KC_RALT
    ),
	[1] = LAYOUT_split_3x6_3(
        KC_TRNS, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_TRNS,
        KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_HOME, KC_UP, KC_END, KC_NO, KC_NO,
        KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
	[2] = LAYOUT_split_3x6_3(
        KC_TRNS, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
        KC_TRNS, RGB_VAI, RGB_HUI, RGB_SAI, RGB_MOD, RGB_TOG, KC_MINUS, KC_EQUAL, KC_LBRC, KC_RBRC, KC_BACKSLASH, KC_GRAVE,
        KC_TRNS, RGB_VAD, RGB_HUD, RGB_SAD, RGB_RMOD, KC_TRNS, S(KC_MINUS), KC_KP_PLUS, S(KC_LBRC), S(KC_RBRC), S(KC_BACKSLASH), S(KC_GRAVE),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
	[3] = LAYOUT_split_3x6_3(
        KC_TRNS, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_TRNS,
        KC_TRNS, KC_F11, KC_F12, KC_F13, KC_F14, KC_F15, KC_TRNS, KC_TRNS, KC_MPLY, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, QK_BOOT, DB_TOGG, EE_CLR, KC_TRNS, KC_TRNS, KC_MRWD, KC_MSTP, KC_MFFD, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};

const int layer1_purple[] = { 39, 42, 43, 47 };
const int layer1_white[] = { 23, 18, 17, 10, 9, 36, 37, 44, 45, 50 };
const int layer1_off[] = { 7, 8, 11, 12, 15, 16, 21, 22, 34, 35, 48, 49, 19, 20, 52 };
const int layer2_yellow[] = { 8,11,12,15,16,19,20,21,22 };
const int layer3_red[] = { 12,15, 20 };
const int layer3_off[] = { 7,21,34,35,38,39,42,43,46,47,48,49,52 };

uint32_t ltime;
int lilli_reveal_step = 0;

bool rgb_matrix_indicators_user(void) {
    uint16_t time;
    switch (get_highest_layer(layer_state)) {
        case 1:
            lilli_set_hsv_colors(LILLI_LEDS(layer1_purple), HSV_PURPLE);
            HSV lilli_white = {0, 0, rgb_matrix_get_val() +10 };
            lilli_set_hsv_colors(LILLI_LEDS(layer1_white), LILLI_HSV(lilli_white));
            lilli_set_hsv_colors(LILLI_LEDS(layer1_off), HSV_OFF);
            break;
        case 2:
            time = g_rgb_timer;
            if (time > ltime + LILLI_REVEAL_SPEED) {
                lilli_reveal_step++;
                ltime = time;
            }
            dprintf("lilli layer2 %lu %d \n", g_rgb_timer, lilli_reveal_step);

            HSV lilli_yellow = {58, 100, rgb_matrix_get_val() +40 };
            lilli_set_hsv_colors(LILLI_LEDS(layer2_yellow), LILLI_HSV(lilli_yellow));
            break;
        case 3:
            time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);
            HSV lilli_pulsing_red = {0, 255, scale8(abs8(sin8(time) - 128) * 2, rgb_matrix_get_val() +40 ) };
            lilli_set_hsv_colors(LILLI_LEDS(layer3_red), LILLI_HSV(lilli_pulsing_red));
            lilli_set_hsv_colors(LILLI_LEDS(layer3_off), HSV_OFF);
            break;
    }
    // #ifdef AUDIO_ENABLE
    // PLAY_SONG(function_layer_sound);
    // #endif // AUDIO_ENABLE
    return true;
}
// void keyboard_post_init_user(void) {
//     debug_enable = true;
//     debug_matrix = true;
// }

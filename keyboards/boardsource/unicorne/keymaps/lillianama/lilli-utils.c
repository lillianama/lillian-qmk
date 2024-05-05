#pragma once

#include <math.h>
#include <stdio.h>
#include "debug.h"
#include "lib/lib8tion/lib8tion.h"
#include "oled_driver.h"
#include "rgb_matrix.h"
#include "wpm.h"

#define LILLI_REVEAL_SPEED 250
#define LILLI_LEDS(leds) leds, sizeof(leds) / sizeof(leds[0])
#define LILLI_HSV(hsv) hsv.h, hsv.s, hsv.v

#define LILLI_WPM_GRAPH_TIMER 100
#define LILLI_WPM_BOX_HEIGHT 50
#define LILLI_WPM_BOX_WIDTH 32
#define LILLI_WPM_BOX_BORDER 1
#define LILLI_WPM_BOX_SPACER 1
#define LILLI_WPM_MAX 110

// struct wpm_buffer {
//     uint8_t index;
//     uint8_t size;
//     uint8_t list[];
// };

uint8_t wpm;
uint8_t scaled_wpm;
//static struct wpm_buffer wpms;

static void lilli_set_hsv_color_multi(const int leds[], int count, uint8_t h, uint8_t s, uint8_t v) {
    HSV hsvcolor = { h, s, v };
    RGB rgbcolor = hsv_to_rgb(hsvcolor);
    for (int i = 0; i < count; i++)
        rgb_matrix_set_color(leds[i], rgbcolor.r, rgbcolor.g, rgbcolor.b);
}

struct wpm_draw_params {
    int w_offset;
    int h_offset;
    int starting_w;
    int starting_h;
    bool write_off_pixel;
};

static void lilli_wpm_draw(struct wpm_draw_params params, bool (*callback)(int w, int h)) {
    bool pixel_on;
    for (int w = params.starting_w; w < LILLI_WPM_BOX_WIDTH - params.w_offset; w++) {
        pixel_on = false;
        for (int h = params.starting_w; h < LILLI_WPM_BOX_HEIGHT - params.h_offset; h++) {
            pixel_on = (*callback)(w, h);
            if ((params.write_off_pixel && !pixel_on) || pixel_on)
                oled_write_pixel(w, h, pixel_on);
        }
    }
}

static bool lilli_get_wpm_border_pixel(int w, int h) {
    if (w == 0 || w == LILLI_WPM_BOX_WIDTH - 1)
        return true;
    else if (h == 0 || h == LILLI_WPM_BOX_HEIGHT - 1)
        return true;
    else
        return false;
}

static bool lilli_get_wpm_graph_pixel(int w, int h) {
    // wpms.list[LILLI_BOX_WIDTH - 2] = scaled_wpm;
    // for (int i = 2; i < LILLI_BOX_WIDTH - 3; i++) {
    //     wpms.list[i] = wpms.list[i+1];
    // }
    // dprintf("lilli_render_wpm_graph() wpm_array length: %d\n", blox_length(wpm_array));
    //  for (int i = 0; i < wpms.size; i++) {
    //      dprintf("%d, ", wpms.list[i]);
    //  }
    if (h > 1 && w > 1 && h < LILLI_WPM_BOX_HEIGHT - 2 && w < LILLI_WPM_BOX_WIDTH - 2) {
        if (abs8(h - LILLI_WPM_BOX_HEIGHT) <= scaled_wpm) return true;
        else return false;
    } else return false;
}

static void lilli_render_wpm_graph(void) {
    static bool initializing = true;
    static uint32_t timer = 0;
    static uint8_t lwpm = 0;

    if (timer_elapsed32(timer) < LILLI_WPM_GRAPH_TIMER) {
        return;
    } else {
        timer = timer_read32();
        dprint("lilli_render_wpm_graph() hi\n");
    }

    if (initializing) {
        initializing = false; //don't try this again :)
        dprintf("lilli_render_wpm_graph() initializing\n");

        // wpms.index = 0;
        // wpms.size = LILLI_WPM_BOX_WIDTH - 4;

        //draw the graph border
        struct wpm_draw_params border_params;
        border_params.h_offset = 0;
        border_params.w_offset = 0;
        border_params.starting_h = 0;
        border_params.starting_w = 0;
        border_params.write_off_pixel = false;
        lilli_wpm_draw(border_params, &lilli_get_wpm_border_pixel);

        //set inital wpm text
        oled_set_cursor(1, 1);
        oled_write("---", false);
    }

    wpm = get_current_wpm();

    //stop here if the wpm hasn't changed since last run
    if (wpm == lwpm) return; else lwpm = wpm;

    //scale the wpm value in proportion to a desired maximum and the height of the graph area
    scaled_wpm = (wpm * (LILLI_WPM_BOX_HEIGHT - 2)) / LILLI_WPM_MAX;

    dprintf("lilli_render_wpm_graph() wpm = %d scaled_wpm = %d \n", wpm, scaled_wpm);

    //draw the graph
    struct wpm_draw_params graph_params;
    graph_params.h_offset = 1;
    graph_params.w_offset = 1;
    graph_params.starting_h = 1;
    graph_params.starting_w = 1;
    graph_params.write_off_pixel = true;
    lilli_wpm_draw(graph_params, &lilli_get_wpm_graph_pixel);

    //write the wpm text
    oled_set_cursor(1, 1);
    char wpm_str[4];
    sprintf(wpm_str, "%03d", wpm);
    oled_write(wpm_str, false);
}

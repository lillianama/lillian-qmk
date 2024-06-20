#pragma once

#include <math.h>
#include <stdio.h>
#include "debug.h"
#include "lib/lib8tion/lib8tion.h"
#include "oled_driver.h"
#include "timer.h"
#include "wpm.h"
#include "lilli-gfx.c"

#define LILLI_WPM_LEVEL_TIMER 100
#define LILLI_WPM_LEVEL_BOX_HEIGHT 50
#define LILLI_WPM_LEVEL_BOX_WIDTH 32
#define LILLI_WPM_LEVEL_BOX_X 0
#define LILLI_WPM_LEVEL_BOX_Y 0
#define LILLI_WPM_LEVEL_BOX_BORDER 1
#define LILLI_WPM_LEVEL_BOX_SPACER 1
#define LILLI_WPM_LEVEL_MAX 120

#define LILLI_WPM_GRAPH_TIMER 250
#define LILLI_WPM_GRAPH_BOX_HEIGHT 50
#define LILLI_WPM_GRAPH_BOX_WIDTH 32
#define LILLI_WPM_GRAPH_BOX_X 0
#define LILLI_WPM_GRAPH_BOX_Y 53
#define LILLI_WPM_GRAPH_BOX_BORDER 1
#define LILLI_WPM_GRAPH_BOX_SPACER 1
#define LILLI_WPM_GRAPH_MAX 120
#define LILLI_WPM_GRAPH_AREA_WIDTH LILLI_WPM_GRAPH_BOX_WIDTH - (LILLI_WPM_GRAPH_BOX_BORDER * 2) - (LILLI_WPM_GRAPH_BOX_SPACER * 2)
#define LILLI_WPM_GRAPH_AREA_HEIGHT LILLI_WPM_GRAPH_BOX_HEIGHT - (LILLI_WPM_GRAPH_BOX_BORDER * 2) - (LILLI_WPM_GRAPH_BOX_SPACER * 2)

//TODO: change this to 1 array with struct as values
struct wpm_buffer {
    uint8_t wpm;
    uint8_t graph_scaled_y;
};
static struct wpm_buffer wpms[LILLI_WPM_GRAPH_AREA_WIDTH];
int8_t level_scaled_y;

static bool lilli_wpm_level_renderFn(uint8_t x, uint8_t y, ll_object *shape) {
    if (y >= level_scaled_y && y % 2 == 0) return true;
    else return false;
}

static bool lilli_wpm_graph_renderFn(uint8_t x, uint8_t y, ll_object *shape) {
    if (y >= wpms[x-2].graph_scaled_y && (y % 2 == 0)) return true;
    else return false;
}

static void lilli_render_wpm_level(void) {
    static bool initializing = true;
    static uint32_t timer = 0;
    static uint8_t lwpm = 0;

    if (timer_elapsed32(timer) < LILLI_WPM_LEVEL_TIMER) {
        return;
    } else {
        timer = timer_read32();
        //dprint("lilli_render_wpm_level() hi\n");
    }

    if (initializing) {
        //dprintf("lilli_render_wpm_level() initializing\n");

        //draw the border
        struct ll_box_params lp = { LILLI_WPM_LEVEL_BOX_BORDER };
        ll_draw_box(
            LILLI_WPM_LEVEL_BOX_X,
            LILLI_WPM_LEVEL_BOX_Y,
            LILLI_WPM_LEVEL_BOX_WIDTH,
            LILLI_WPM_LEVEL_BOX_HEIGHT,
            true,
            lp
        );

        //set inital wpm text
        oled_set_cursor(1, 1);
        oled_write("---", false);

        initializing = false;
    }

    int wpm = get_current_wpm();

    //stop here if the wpm hasn't changed since last run
    if (wpm == lwpm) return; else lwpm = wpm;

    //create lilli gfx shape for level
    ll_object level = {
        LILLI_WPM_LEVEL_BOX_X + LILLI_WPM_LEVEL_BOX_BORDER + LILLI_WPM_LEVEL_BOX_SPACER,
        LILLI_WPM_LEVEL_BOX_Y + LILLI_WPM_LEVEL_BOX_BORDER + LILLI_WPM_LEVEL_BOX_SPACER,
        LILLI_WPM_LEVEL_BOX_WIDTH - ((LILLI_WPM_LEVEL_BOX_BORDER + LILLI_WPM_LEVEL_BOX_SPACER) * 2),
        LILLI_WPM_LEVEL_BOX_HEIGHT - ((LILLI_WPM_LEVEL_BOX_BORDER + LILLI_WPM_LEVEL_BOX_SPACER) * 2),
        true,
        false,
        lilli_wpm_level_renderFn
    };

    //scale the wpm value in proportion to a desired maximum wpm and the height of the graph area
    const int y_gt = LILLI_WPM_LEVEL_BOX_Y + 2;
    const int y_gb = LILLI_WPM_LEVEL_BOX_Y + LILLI_WPM_LEVEL_BOX_HEIGHT - 2;
    const int gh = y_gb - y_gt;
    const int s = (wpm * gh) / LILLI_WPM_LEVEL_MAX;
    level_scaled_y = gh - s + y_gt;

    //draw the level
    ll_draw_object(level);

    //write the wpm text
    oled_set_cursor(1, 1);
    char wpm_str[] = "---";
    sprintf(wpm_str, "%03u", wpm);
    oled_write(wpm_str, false);
}

static void lilli_render_wpm_graph(void) {
    static bool initializing = true;
    static uint32_t timer = 0;

    if (timer_elapsed32(timer) < LILLI_WPM_GRAPH_TIMER) {
        return;
    } else {
        timer = timer_read32();
        //dprint("lilli_render_wpm_graph() hi\n");
    }

    if (initializing) {
        initializing = false; //don't try this again :)
        //dprintf("lilli_render_wpm_graph() initializing\n");

        //init the arrays
        struct wpm_buffer val = { 0, LILLI_WPM_GRAPH_BOX_Y + LILLI_WPM_GRAPH_BOX_HEIGHT - 2};
        for (int i = 0; i < LILLI_WPM_GRAPH_AREA_WIDTH; i++) {
            wpms[i] = val;
        }

        //draw the graph border
        struct ll_box_params lp = { LILLI_WPM_GRAPH_BOX_BORDER };
        ll_draw_box(
            LILLI_WPM_GRAPH_BOX_X,
            LILLI_WPM_GRAPH_BOX_Y,
            LILLI_WPM_GRAPH_BOX_WIDTH,
            LILLI_WPM_GRAPH_BOX_HEIGHT,
            true,
            lp
        );
    }

    int wpm = get_current_wpm();

    static int sum = 0;
    sum += wpm;

    if (sum == 0) return; //nothing to do

    //scale the wpm to a y coord value in the graph
    //  this value is calculated as a proportion to a desired maximum wpm and the height of the graph area
    const int y_graph_top = LILLI_WPM_GRAPH_BOX_Y + 2;
    const int y_graph_bottom = LILLI_WPM_GRAPH_BOX_Y + LILLI_WPM_GRAPH_BOX_HEIGHT - 2;
    const int graph_height = y_graph_bottom - y_graph_top;
    const int scaled_height = floor((wpm * graph_height) / LILLI_WPM_GRAPH_MAX);
    const uint8_t graph_y = graph_height - scaled_height + y_graph_top;

    //shift arrays and sum
    sum -= wpms[0].wpm;
    memmove8(&wpms[0], &wpms[1], sizeof(wpms[0]) * (LILLI_WPM_GRAPH_AREA_WIDTH - 1));

    //push latest values
    struct wpm_buffer val = { wpm, graph_y };
    wpms[LILLI_WPM_GRAPH_AREA_WIDTH - 1] = val;

    //create a lilli gfx object
    ll_object graph = {
        LILLI_WPM_GRAPH_BOX_X + LILLI_WPM_GRAPH_BOX_BORDER + LILLI_WPM_GRAPH_BOX_SPACER,
        LILLI_WPM_GRAPH_BOX_Y + LILLI_WPM_GRAPH_BOX_BORDER + LILLI_WPM_GRAPH_BOX_SPACER,
        LILLI_WPM_GRAPH_AREA_WIDTH,
        LILLI_WPM_GRAPH_AREA_HEIGHT,
        true,
        false,
        lilli_wpm_graph_renderFn
    };
    ll_draw_object(graph);

    //write the avg
    int avg = sum / (LILLI_WPM_GRAPH_AREA_WIDTH + 1);
    oled_set_cursor(1, 13);
    oled_write("avg", false);
    char wpm_str[8];
    sprintf(wpm_str, "%03u", avg);
    oled_set_cursor(1, 14);
    oled_write(wpm_str, false);
}

static void lilli_render_line(void) {
    static bool initializing = true;
    static uint32_t timer = 0;
    static bool direction = false;

    struct line {
        uint8_t x0, y0, x1, y1;
    };

    static struct line lline = {};

    if (timer_elapsed32(timer) < 50) {
        return;
    } else {
        timer = timer_read32();
        //dprint("lilli_render_wpm_level() hi\n");
    }

    if (initializing) {
        initializing = false;

        lline.x0 = 10;
        lline.y0 = 0;
        lline.x1 = 22;
        lline.y1 = 127;
    }

    //erase the last line
    ll_draw_line(lline.x0, lline.y0, lline.x1, lline.y1, false);

    if (lline.y0 == 127 || lline.y0 == 0) direction = !direction;
    if (direction) {
        lline.y0++;
        lline.y1--;
    } else {
        lline.y0--;
        lline.y1++;
    }

    //draw the new line
    ll_draw_line(lline.x0, lline.y0, lline.x1, lline.y1, true);
}

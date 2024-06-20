#pragma once

#include "oled_driver.h"
#include "lib/lib8tion/lib8tion.h"

typedef struct ll_object ll_object;
struct ll_object {
    uint8_t x, y, w, h;
    bool on;
    bool transparent;
    bool (*renderFn)(uint8_t x, uint8_t y, ll_object *shape);
    void *params;
};

static void ll_draw_point(uint8_t x, uint8_t y, bool on) {
    oled_write_pixel(x, y, on);
}

static void ll_draw_object(ll_object shape) {
    bool pixel_on;
    for (int x = shape.x; x < shape.x + shape.w; x++) {
        pixel_on = false;
        for (int y = shape.y; y < shape.y + shape.h; y++) {
            pixel_on = shape.renderFn(x, y, &shape);
            if (!shape.transparent && !pixel_on)
                ll_draw_point(x, y, false);
            else if (pixel_on) ll_draw_point(x, y, shape.on);
        }
    }
}

struct ll_box_params {
    int border_width;
};
static bool ll_box_renderFn(uint8_t x, uint8_t y, ll_object *shape) {
    if (!shape->params) return false; //params required for this fn
    struct ll_box_params *params;
    params = (struct ll_box_params *)shape->params;

    if ((x >= shape->x && x <= shape->x + params->border_width - 1) || (x == shape->x + shape->w - 1))
        return true;
    else if ((y >= shape->y && y <= shape->y + params->border_width - 1) || (y == shape->y + shape->h - 1))
        return true;
    else
        return false;
}

static void ll_draw_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool on, struct ll_box_params params) {
    ll_object box = {
        x, y, w, h,
        on,
        true,
        ll_box_renderFn,
        (void *)&params
    };
    ll_draw_object(box);
}

static void ll_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool on) {
    //Bresenham’s line algorithm
    int16_t dx = abs8(x1 - x0);
    int16_t dy = -abs8(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;
    int16_t e2;

    while (true) {
        ll_draw_point(x0, y0, on);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

static void ll_draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, bool on) {
    //Bresenham’s midpoint circle algorithm
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 1 - radius;  // Improved initialization for the error term

    while (x >= y) {
        // Draw the pixels in all eight octants
        ll_draw_point(x0 + x, y0 + y, on);
        ll_draw_point(x0 + y, y0 + x, on);
        ll_draw_point(x0 - y, y0 + x, on);
        ll_draw_point(x0 - x, y0 + y, on);
        ll_draw_point(x0 - x, y0 - y, on);
        ll_draw_point(x0 - y, y0 - x, on);
        ll_draw_point(x0 + y, y0 - x, on);
        ll_draw_point(x0 + x, y0 - y, on);

        y += 1;
        if (err <= 0) {
            err += 2 * y + 1;
        } else {
            x -= 1;
            err += 2 * (y - x + 1);
        }
    }
}

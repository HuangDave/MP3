/*
 * gfx.c
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#include "gfx.h"

Color ColorMake(uint8_t r, uint8_t g, uint8_t b) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = g;
    return c;
}

Point2D Point2DMake(int16_t x, int16_t y) {
    Point2D p;
    p.x = x;
    p.y = y;
    return p;
}

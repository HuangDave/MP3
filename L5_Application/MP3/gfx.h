
#ifndef GFX_H
#define GFX_H

#include "LPC17xx.h"

#define WHITE_COLOR Color { 0xFF, 0xFF, 0xFF }
#define RED_COLOR   Color { 0xFF, 0x00, 0x00 }
#define GREEN_COLOR Color { 0x00, 0xFF, 0x00 }
#define BLUE_COLOR  Color { 0x00, 0x00, 0xFF }
#define BLACK_COLOR Color { 0x00, 0x00, 0x00 }

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct {
    int16_t x;
    int16_t y;
} Point2D;

typedef struct {
    uint8_t width;
    uint8_t height;
} Size2D;

typedef union {
    struct {
        Point2D origin;
        Size2D  size;
    };
    struct {
        int16_t x;
        int16_t y;
        uint8_t width;
        uint8_t height;
    };
} Frame;

#endif

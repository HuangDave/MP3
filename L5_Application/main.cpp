/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include <stdint.h>
#include <stdio.h>

#include "tasks.hpp"
#include "FreeRTOS.h"
#include "queue.h"
#include "storage.hpp"
#include "io.hpp"

#include "MP3/ST7735.hpp"
#include "MP3/VS1053B.hpp"

QueueHandle_t streamQueue;

int main(void) {
    streamQueue = xQueueCreate(32, sizeof(uint8_t));

    MP3.setVolume(150);

    Point2D p0 = Point2D { 1, 1 };
    Point2D p1 = Point2D { 5, 100 };
    LCDDisplay.drawLine(p0, p1, Color { 0xFF, 0, 0 });

    //while(1);

    uint8_t *data = new uint8_t(9000);
    FRESULT result = Storage::read("1:testmusic.mp3", data, 9000 - 1);

    uint8_t buffer[32];
    for (uint8_t i = 0; i < 9000; i++) {
        if (i != 0 && !(i % 32)) {
            MP3.buffer(data, 32);
        }
        buffer[i] = data[i];

    }
    return 0;
}

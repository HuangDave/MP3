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
#include "utilities.h"

#include "MP3/ST7735.hpp"
#include "MP3/VS1053B.hpp"

QueueHandle_t streamQueue;

int main(void) {
    //Point2D p0 = Point2D { 1, 1 };
    //Point2D p1 = Point2D { 5, 100 };
    //LCDDisplay.drawLine(p0, p1, Color { 0xFF, 0, 0 });

    const uint32_t fileSize = 1024 * 6;
    uint8_t *data = new uint8_t[fileSize];
    FRESULT result = Storage::read("1:easy128.mp3", data, fileSize - 1);
    printf("%s\n", result == FR_OK ? "FILE OK" : "FILE NOT OK");

    for (uint32_t i = 32; i < fileSize; i = i + 32) {
        MP3.buffer(data, 32);
        data += 32;
        delay_ms(50);
    }

    printf("\n");

    printf("status: %x\n", MP3.readREG(0x1));
    printf("mode %x\n",    MP3.readREG(0x0));
    printf("clockf: %x\n", MP3.readREG(0x3));
    printf("AUDATA %x\n",  MP3.readREG(0x5));
    printf("HDAT0: %x\n",  MP3.readREG(0x8));
    printf("HDAT1: %x\n",  MP3.readREG(0x9));
    printf("vol %x\n",     MP3.readREG(0xB));

    while (1);
    return 0;
}

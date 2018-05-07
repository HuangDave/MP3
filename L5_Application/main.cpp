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

#include "MP3/UITableView.hpp"

QueueHandle_t streamQueue;

char *list[] {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};
uint32_t songCount = 10;

//UITableView<char> songMenu = UITableView<char>(Frame { 0, 0, ST7735_TFT_WIDTH, ST7735ST7735_TFT_HEIGHT});

void updateSongeItem(UITableViewCell &cell, uint32_t index) {

}

void setupSongMenu() {

    UITableView menu = UITableView(Frame { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
    menu.setRowHeight(10);
    menu.setMininmumRows(5);
    menu.setItemCount(songCount);
    menu.attachCellUpdateHandler(&updateSongeItem);
    menu.update();
}

void printHDAT() {

    VS1053B::HeaderData hdat = MP3.getHDAT();
    printf("\nHeader Data\n");
    printf("Stream Valid: %s\n", (hdat.syncword == 2047) ? "Valid" : "Not Valid");

    uint8_t id = hdat.id;
    printf("ID: %s\n", id == 3 ? "ISO 11172-3 MPG 1.0" :
    id == 2 ? "ISO 13818-3 MPG 2.0 (1/2-rate)" :
    id == 1 ? "MPG 2.5 (1/4-rate)" :
    "MPG 2.5 (1/4-rate)" );

    uint8_t layer = hdat.layer;
    printf("Layer: %s\n", layer == 3 ? "1" :
    (layer == 2 ? "2" :
    "3"));

    uint8_t mode = hdat.mode;
    printf("Mode: %s\n", mode == 3 ? "mono" :
    (mode == 2 ? "dual channel" :
    (mode == 1 ? "joint stereo" :
    "stereo")));

    printf("Bit Rate: %d\n", hdat.bitrate);

    uint8_t sampleRate = hdat.samplerate;
    printf("Sample Rate: %d\n", VS1053B::sampleRateLUT[sampleRate][id]);
}

int main(void) {


    //setupSongMenu();

    /*
    Point2D p0 = Point2D { 1, 1 };
    Point2D p1 = Point2D { 5, 100 };
    LCDDisplay.drawLine(p0, p1, Color { 0xFF, 0, 0 });

    uint8_t font[] = {
            0x7C, 0x12, 0x11, 0x12, 0x7C
    };
    LCDDisplay.drawFont(Point2D{1,1}, font, BLACK_COLOR, GREEN_COLOR);

    uint8_t font2[] = {
            0x7F, 0x49, 0x49, 0x49, 0x36
    };
    LCDDisplay.drawFont(Point2D{7,1}, font2, BLACK_COLOR, GREEN_COLOR);

    delay_ms(1000);

    LCDDisplay.drawFont(Point2D{7,1}, font2, BLACK_COLOR, RED_COLOR); */

//    printf("status: %x\n", MP3.readREG(0x1));
//    printf("mode %x\n",    MP3.readREG(0x0));
//    printf("clockf: %x\n", MP3.readREG(0x3));
//    printf("AUDATA %x\n",  MP3.readREG(0x5));
//    printf("HDAT0: %x\n",  MP3.readREG(0x8));
//    printf("HDAT1: %x\n",  MP3.readREG(0x9));
//    printf("vol %x\n",     MP3.readREG(0xB));

    /*
    const uint32_t fileSize = 1000 * 7;
    uint8_t *data = new uint8_t[fileSize];
    FRESULT result = Storage::read("1:44_32_32.mp3", data, fileSize - 1);
    printf("%s\n", result == FR_OK ? "FILE OK" : "FILE NOT OK");

    MP3.setVolume(225);
    printf("byteRate: %d", MP3.getByteRate());
    MP3.playSong();

    for (uint32_t i = 32; i < fileSize; i = i + 32) {
        MP3.buffer(data, 32);
        data += 32;
        //printf("HDAT0: %x\n",  MP3.readREG(0x8));
        //printf("HDAT1: %x\n",  MP3.readREG(0x9));
    }

    //MP3.sendEndFillBytes();

    printf("\n");

    printf("status: %x\n", MP3.readREG(0x1));
    printf("mode %x\n",    MP3.readREG(0x0));
    printf("clockf: %x\n", MP3.readREG(0x3));
    printf("AUDATA %x\n",  MP3.readREG(0x5));
    printf("HDAT0: %x\n",  MP3.readREG(0x8));
    printf("HDAT1: %x\n",  MP3.readREG(0x9));
    printf("vol %x\n",     MP3.readREG(0xB));

    printHDAT(); */

    while (1);
    return 0;
}

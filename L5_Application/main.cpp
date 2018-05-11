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
#include <string.h>

#include "tasks.hpp"
#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"

#include "storage.hpp"
#include "io.hpp"

#include "utilities.h"

#include "MP3/UserInterface.hpp"

#include "MP3/Drivers/ST7735.hpp"
#include "MP3/Drivers/VS1053B.hpp"
#include "MP3/MusicPlayer.hpp"
#include "MP3/UI/UITableView.hpp"

#include "ff.h"

#include <iostream>
#include "printf_lib.h"

QueueHandle_t queue;

UserInterface *ui;
MusicPlayer *player;

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

void testAudio() {
    /*
    printf("status: %x\n", MP3.readREG(0x1));
    printf("mode %x\n",    MP3.readREG(0x0));
    printf("clockf: %x\n", MP3.readREG(0x3));
    printf("AUDATA %x\n",  MP3.readREG(0x5));
    printf("HDAT0: %x\n",  MP3.readREG(0x8));
    printf("HDAT1: %x\n",  MP3.readREG(0x9));
    printf("vol %x\n",     MP3.readREG(0xB));
    */

    const uint32_t fileSize = 1024 * 1000 * 2.850;
    //uint8_t *data = new uint8_t[fileSize];

    //FRESULT result = Storage::read("1:44_128.mp3", data, fileSize - 1);
    //printf("%s\n\n", result == FR_OK ? "FILE OK" : "FILE NOT OK");

    MP3.setVolume(35);

    for (uint32_t i = 0; i < fileSize/512; i++) {
        uint8_t data[512] = { 0 };
        Storage::read("1:44_128_32.mp3", data, 512-1, i * 512);
        for (uint32_t j = 0; j < 512/32; j++){
            MP3.buffer(data + (j*32), 32);
        }
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

    printHDAT();

    while (1);
}

int main(void) {


    // Initialize player
    //player = new MusicPlayer();
    //ui = new UserInterface(PRIORITY_HIGH);

    //scheduler_add_task(player);
    //scheduler_add_task(ui);
    //scheduler_start();

    testAudio();

    return 0;
}

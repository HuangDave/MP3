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

UserInterface *ui;
MusicPlayer *player;

QueueHandle_t queue;

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

void fetch(void *) {

    const uint32_t fileSize = 1024 * 1000 * 11.074; //2.850;

    MP3.setVolume(235);
    MP3.enablePlayback();

    const uint32_t size = 1024;

    while (1) {

        for (uint32_t i = 0; i < fileSize/size; i++) {
            uint8_t data[size] = { 0 };
            Storage::read("1:rain_320.mp3", data, size, i * size);
            xQueueSend(queue, data, portMAX_DELAY);
            vTaskDelay(10);
        }
        vTaskSuspend(0);
    }
}

void play(void *) {
    const uint32_t size = 1024;

    while (1) {
        uint8_t data[size] = { 0 };
        if (xQueueReceive(queue, data, portMAX_DELAY)) {
            for (uint32_t j = 0; j < size/32; j++){
                MP3.buffer(data + (j*32), 32);
            }
        }
    }
}

void testAudio() {
    const uint32_t fileSize = 1024 * 1000 * 11.074; //2.850;

    MP3.setVolume(235);
    MP3.enablePlayback();

    uint32_t size = 1024;

    uint8_t data[1024] = { 0 };
    for (uint32_t i = 0; i < fileSize/size; i++) {
        Storage::read("1:rain_320.mp3", data, size, i * size);
        for (uint32_t j = 0; j < size/32; j++){
            MP3.buffer(data + (j*32), 32);
        }
        delay_ms(10);
    }
    while (1);
}

int main(void) {
    MP3.setVolume(235);
    MP3.enablePlayback();

    queue = xQueueCreate(3, sizeof(uint8_t) * 1024);

    uint32_t STACK_SIZE = 1024 * 5;

    //xTaskCreate(fetch, "fetch", STACK_SIZE, (void *)0, 1, 0 );
    //xTaskCreate(play,  "play",  STACK_SIZE, (void *)0, 1, 0 );

    //vTaskStartScheduler();

    // Initialize player
    player = new MusicPlayer();
    //ui = new UserInterface(PRIORITY_HIGH);

    //scheduler_add_task(player);
    //scheduler_add_task(ui);




    scheduler_start();

    //testAudio();



    return 0;
}

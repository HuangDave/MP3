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

char *list[] {
    "t8vvUZ 8zHPJvS2n",
    "ZOfznksXVxR1kp9",
    "dUC7le4zO6jngAM",
    "azY ZIKHGGXgXLzv",
    "OQLZu3ZDxCaLYQI",
    "Ss13LembrMtn IjnC",
    "EmUVTm7TNEwoFRn",
    "mXHSnzuhRCuNQ80",
    "pRqUnYZYQCf4pqf",
    "ckt dtqINXOPHEcV"
};
static uint32_t songCount = 10;

UserInterface *ui;
MusicPlayer *player;

UITableView *mpSongMenu;
UIView *mpNowPlaying;

void updateSongeItem(UITableViewCell &cell, uint32_t index) {
    //printf("cursor on: %s", list[index]);
    cell.setText(list[index], strlen(list[index]));
}

void userDidSelectSong(uint32_t index) {
    printf("playing song: %s\n", list[index]);
    //player->play(list[index]);
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

    // Initialize player
    player = new MusicPlayer();
    ui = new UserInterface(PRIORITY_HIGH);

    //ui->songMenu().setItemCount(songCount);


    //scheduler_add_task(player);
    scheduler_add_task(ui);
    scheduler_start();

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

    return 0;
}

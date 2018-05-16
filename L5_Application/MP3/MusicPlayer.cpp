/*
 * MusicPlayer.cpp
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#include <MP3/MusicPlayer.hpp>

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <iostream>
#include "ff.h"

// TODO: remove
#include "storage.hpp"
#include "io.hpp"

#include "printf_lib.h"

#define STREAM_QUEUE_SIZE        (3)
#define STREAM_QUEUE_BUFFER_SIZE (1024)

#define DECODER_BUFFER_SIZE      (32)

#define SONG_QUEUE_SIZE          (2)

MusicPlayer* MusicPlayer::instance = NULL;

MusicPlayer& MusicPlayer::sharedInstance() {
    if (instance == NULL) instance = new MusicPlayer();
    return *instance;
}

MusicPlayer::MusicPlayer() {
    mStreamQueue = xQueueCreate(STREAM_QUEUE_SIZE, sizeof(uint8_t) * STREAM_QUEUE_BUFFER_SIZE);
    mSongQueue   = xQueueCreate(SONG_QUEUE_SIZE,   sizeof(SongInfo));

    mPlaySema    = xSemaphoreCreateBinary();

    mpCurrentSongName = NULL;

    // set default volume to 50 on startup
    mVolume = 90;
    setVolume(mVolume);

    scheduler_add_task(new BufferMusicTask(PRIORITY_LOW, mSongQueue, mStreamQueue));
    scheduler_add_task(new StreamMusicTask(PRIORITY_LOW, mStreamQueue));
}

MusicPlayer::~MusicPlayer() { }

void MusicPlayer::queue(SongInfo *song) {
    mDecoder.enablePlayback();


    if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {

        const char *path = song->path;

        // get file size
        FILE *f = fopen(path, "r"); // read current song in SD Card
        fseek(f, 0, SEEK_END);
        const long fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
        fclose(f);

        song->fileSize = fileSize;

        xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
        xQueueSend(mSongQueue, song, portMAX_DELAY);
    }
}

void MusicPlayer::pause() {
    // TODO: empty queue
    mDecoder.disablePlayback();
}

void MusicPlayer::resume() {
    mDecoder.enablePlayback();
    xSemaphoreGive(mPlaySema);
}

void MusicPlayer::incrementVolume() {
    mVolume += 5;
    setVolume(mVolume);
}

void MusicPlayer::decrementVolume() {
    mVolume -= 5;
    setVolume(mVolume);
}

inline void MusicPlayer::setVolume(uint8_t percentage) {
    // clamp percentage to 0 or 100
    if      (mVolume < 0)   mVolume = 0;
    else if (mVolume > 100) mVolume = 100;
    mDecoder.setVolume( (mVolume/100.0) * VS1053B_MAX_VOL );
}

bool MusicPlayer::BufferMusicTask::run(void *) {
    // TODO: should stream selected song

    /*
    const uint32_t bufferSize = STREAM_QUEUE_BUFFER_SIZE;

    while (1) {
        SongInfo *song = NULL;
        if (xQueueReceive(mSongQueue, song, portMAX_DELAY)) {
            mpCurrentSong = song;

            const uint32_t fileSize = song->fileSize;
            const uint32_t bufferSize = STREAM_QUEUE_BUFFER_SIZE;

            for (uint32_t i = 0; i < fileSize/bufferSize; i++) {
                uint8_t data[bufferSize] = { 0 };

                switch (mDecoder.getState()) {
                    case PAUSED: xSemaphoreTake(mPlaySema, portMAX_DELAY); break;
                    default: break;
                }

                if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {
                    Storage::read("1:rain_320.mp3", data, bufferSize, i * bufferSize);

                    xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
                    xQueueSend(mStreamQueue, data, portMAX_DELAY);
                }
                vTaskDelay(15);
            }
        }
    }
*/

    const uint32_t fileSize = 1024 * 1000 * 11.074;
    const uint32_t size = STREAM_QUEUE_BUFFER_SIZE;
    mDecoder.enablePlayback();
    while (1) {
        for (uint32_t i = 0; i < fileSize/size; i++) {
            uint8_t data[size] = { 0 };
            if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {

                Storage::read("1:light_years.mp3", data, size, i * size);

                xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
                xQueueSend(mStreamQueue, data, portMAX_DELAY);
            }
            vTaskDelay(15);
        }
    }

    return true;
}

bool MusicPlayer::StreamMusicTask::run(void *) {
    const uint32_t size = STREAM_QUEUE_BUFFER_SIZE;
    const uint32_t buffSize = DECODER_BUFFER_SIZE;

    while (1) {
        uint8_t data[size] = { 0 };
        if (xQueueReceive(mStreamQueue, data, portMAX_DELAY)) {

            for (uint32_t j = 0; j < size/buffSize; j++){

                //while (mDecoder.getState() == VS1053B::PAUSED) vTaskDelay(1);

                MP3.buffer(data + (j*buffSize), buffSize);
            }
            vTaskDelay(15);
        }
    }

    return true;
}

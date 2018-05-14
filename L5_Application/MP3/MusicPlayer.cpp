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

#define STREAM_QUEUE_SIZE        (3)
#define STREAM_QUEUE_BUFFER_SIZE (1024)

#define SONG_QUEUE_SIZE          (2)

MusicPlayer::MusicPlayer() {
    mStreamQueue = xQueueCreate(STREAM_QUEUE_SIZE, sizeof(uint8_t) * STREAM_QUEUE_BUFFER_SIZE);
    mSongQueue   = xQueueCreate(SONG_QUEUE_SIZE,   sizeof(SongInfo));

    mpCurrentSongName = NULL;

    // set default volume to 50 on startup
    mVolume = 50;
    setVolume(mVolume);

    scheduler_add_task(new BufferMusicTask(PRIORITY_LOW, mSongQueue, mStreamQueue));
    scheduler_add_task(new StreamMusicTask(PRIORITY_LOW, mStreamQueue));
}

MusicPlayer::~MusicPlayer() { }

void MusicPlayer::play(SongInfo *song) {
    mDecoder.enablePlayback();

    mpCurrentSongName = song->name;

    const char dirPrefix[] = "1:";
    char *fileName = (char *)malloc(strlen(dirPrefix) + strlen(mpCurrentSongName) - 1);
    strcpy(fileName, dirPrefix);
    strcat(fileName, mpCurrentSongName);

    FILE *f = fopen(fileName, "r"); // read current song in SD Card

    // get file size
    fseek(f, 0, SEEK_END);
    const uint32_t fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    (*song).fileSize = fileSize;

    fclose(f);

/*
    // queue 512 bytes for decoder
    for (uint32_t i = 0; i < fileSize / 512; i++) {
        //if (mState & (VS1053B::STOPPED | VS1053B::CANCELLING)) break;

        uint8_t *data = new uint8_t[512];
        fread(data, 1, 512, f);
        xQueueSend(mStreamQueue, &data, portMAX_DELAY);
    }

    fclose(f); */
}

void MusicPlayer::queue(SongInfo *song) {
    mDecoder.enablePlayback();

    const char *songName = song->path;

    const char dirPrefix[] = "1:";
    char *fileName = (char *)malloc(strlen(dirPrefix) + strlen(songName) + 1);
    strcpy(fileName, dirPrefix);
    strcat(fileName, songName);

    FILE *f = fopen(fileName, "r"); // read current song in SD Card

    // get file size
    fseek(f, 0, SEEK_END);
    const uint32_t fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    (*song).fileSize = fileSize;

    fclose(f);

    xQueueSend(mSongQueue, song, portMAX_DELAY);
}

void MusicPlayer::pause() {
    // TODO: empty queue
    mDecoder.disablePlayback();
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
    if (mVolume < 0)        mVolume = 0;
    else if (mVolume > 100) mVolume = 100;
    mDecoder.setVolume( (mVolume/100.0) * VS1053B_MAX_VOL );
}

bool MusicPlayer::BufferMusicTask::run(void *) {
    const uint32_t fileSize = 1024 * 1000 * 11.074;
    const uint32_t size = STREAM_QUEUE_BUFFER_SIZE;

    while (1) {

        /* TODO: should stream selected song

        SongInfo *song = NULL;
        if (xQueueReceive(mSongQueue, song, portMAX_DELAY)) {
            mpCurrentSong = song;

            const fileSize = song->fileSize;

            for (uint32_t i = 0; i < fileSize/size; i++) {
                uint8_t data[size] = { 0 };
                if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {
                    Storage::read("1:rain_320.mp3", data, size, i * size);

                    xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
                    xQueueSend(mStreamQueue, data, portMAX_DELAY);
                }
                vTaskDelay(15);
            }
        } */

        for (uint32_t i = 0; i < fileSize/size; i++) {
            uint8_t data[size] = { 0 };
            if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {

                Storage::read("1:rain_320.mp3", data, size, i * size);

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

    while (1) {
        uint8_t data[size] = { 0 };
        if (xQueueReceive(mStreamQueue, data, portMAX_DELAY)) {

            for (uint32_t j = 0; j < size/32; j++){

                while (mDecoder.getState() == VS1053B::PAUSED) vTaskDelay(1);

                MP3.buffer(data + (j*32), 32);
            }
            vTaskDelay(15);
        }
    }

    return true;
}

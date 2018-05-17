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
#include "ff.h"
#include "storage.hpp"

#define STREAM_QUEUE_SIZE        (3)
#define STREAM_QUEUE_BUFFER_SIZE (1024)

#define SONG_QUEUE_SIZE          (2)

MusicPlayer* MusicPlayer::instance = NULL;

MusicPlayer& MusicPlayer::sharedInstance() {
    if (instance == NULL) instance = new MusicPlayer();
    return *instance;
}

MusicPlayer::MusicPlayer() {
    mStreamQueue = xQueueCreate(STREAM_QUEUE_SIZE, sizeof(uint8_t) * STREAM_QUEUE_BUFFER_SIZE);
    mSongQueue   = xQueueCreate(SONG_QUEUE_SIZE,   sizeof(SongInfo));

    //mPlaySema    = xSemaphoreCreateBinary();

    // set default volume to 80 on startup
    mVolume = 80;
    setVolume(mVolume);

    fetchSongs();

    bufferTask = new BufferMusicTask(PRIORITY_LOW, mSongQueue, mStreamQueue);

    scheduler_add_task(bufferTask);
    scheduler_add_task(new StreamMusicTask(PRIORITY_LOW, mStreamQueue));
}

MusicPlayer::~MusicPlayer() { }

void MusicPlayer::fetchSongs() {
    mSongList.empty();

    const char dirPath[] = "1:";
    DIR directory;
    static FILINFO fileInfo;

#if _USE_LFN
    char lfnBuffer[_MAX_LFN];
    fileInfo.lfsize = _MAX_LFN-1;
    fileInfo.lfname = lfnBuffer;
#endif

    const char *mp3[] = { ".mp3", ".MP3" };
    const char *ext   = strrchr(fileInfo.fname,'.');

    FRESULT res = f_opendir(&directory, dirPath);

    if (res == FR_OK) {

        while (1) {
            fileInfo.lfsize = _MAX_LFN-1;
            fileInfo.lfname = lfnBuffer;

            res = f_readdir(&directory, &fileInfo);

            if (res != FR_OK || fileInfo.fname[0] == 0) break;

            if (!(fileInfo.fattrib & AM_DIR) && (strcmp(ext, mp3[0]) || strcmp(ext, mp3[1]))) {

                SongInfo song;

                const char *fullName = fileInfo.lfname[0] == 0 ? fileInfo.fname : fileInfo.lfname;

                // construct and save full file path by combining directory path and full file name...
                uint32_t len = strlen(dirPath) + strlen(fullName) + 1;
                char *path = new char[len];
                strcpy(path, dirPath);
                strcat(path, fullName);
                path[len-1] = '\0'; // set terminal char at the end of string
                song.path = path;

                // parse and save song name without extension...
                len = strlen(fullName) - strlen(mp3[0]) + 1;
                char *name = new char[len];
                strncpy(name, fullName, len);
                name[len-1] = '\0'; // set terminal char at the end of string
                song.name = name;

                song.fileSize = fileInfo.fsize;

                mSongList.push_back(song);
            }
        }
        f_closedir(&directory);
    }
}

void MusicPlayer::queue(SongInfo *song) {
    mDecoder.enablePlayback();

    if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {
        bufferTask->newSongSelected = true;
        xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
        xQueueSend(mSongQueue, &song, portMAX_DELAY);
    }
}

bool MusicPlayer::pause() {
    // TODO: empty queue
    mDecoder.disablePlayback();
    return false;
}

bool MusicPlayer::resume() {
    mDecoder.enablePlayback();
    //xSemaphoreGive(mPlaySema);
    return true;
}

void MusicPlayer::playNext() {
    // TODO: implement
}

void MusicPlayer::playPrevious() {
    // TODO: implement
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

// UITableViewDataSource Implementation

inline uint32_t MusicPlayer::numberOfItems() const {
    return mSongList.size();
}

inline void MusicPlayer::cellForIndex(UITableViewCell &cell, uint32_t index) {
    SongInfo info = mSongList.at(index);
    cell.setText(info.name, strlen(info.name));
}

// BufferMusicTask Implementation

bool MusicPlayer::BufferMusicTask::run(void *) {
    // TODO: should stream selected song

    const uint32_t bufferSize = STREAM_QUEUE_BUFFER_SIZE;

    while (1) {
        SongInfo *song = NULL;
        if (xQueueReceive(mSongQueue, &song, portMAX_DELAY)) {

            const uint32_t fileSize = song->fileSize;
            const char *path = song->path;

            newSongSelected = false;

            for (uint32_t i = 0; i < fileSize/bufferSize; i++) {
                uint8_t data[bufferSize] = { 0 };

                //switch (mDecoder.getState()) {
                //    case PAUSED: xSemaphoreTake(mPlaySema, portMAX_DELAY); break;
                //    default: break;
                //}

                if (newSongSelected) break;

                if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {
                    Storage::read(path, data, bufferSize, i * bufferSize);
                    xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
                    xQueueSend(mStreamQueue, data, portMAX_DELAY);
                }
                vTaskDelay(15);
            }
        }
    }

    return true;
}

// StreamMusicTask Implementation

bool MusicPlayer::StreamMusicTask::run(void *) {
    const uint32_t size     = STREAM_QUEUE_BUFFER_SIZE;
    const uint32_t buffSize = VS1053B_BUFFER_SIZE;

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

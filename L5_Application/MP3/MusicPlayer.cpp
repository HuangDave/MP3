/*
 * MusicPlayer.cpp
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#include <MP3/MusicPlayer.hpp>

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
    mpDelegate = NULL;

    mPlayMutex   = xSemaphoreCreateMutex();
    //mPlaySema    = xSemaphoreCreateBinary();

    mState = STOPPED;

    // set default volume to 80 on startup
    mVolume = 80;
    setVolume(mVolume);

    fetchSongs();

    mSongIndex = 0;

    // init buffer and streaming tasks...
    QueueHandle_t streamQueue = xQueueCreate(STREAM_QUEUE_SIZE, sizeof(uint8_t) * STREAM_QUEUE_BUFFER_SIZE);
    mSongQueue   = xQueueCreate(SONG_QUEUE_SIZE,   sizeof(SongInfo));

    bufferTask = new BufferMusicTask(PRIORITY_LOW, mSongQueue, streamQueue);
    bufferTask->player = this;
    scheduler_add_task(bufferTask);
    scheduler_add_task(new StreamMusicTask(PRIORITY_LOW, streamQueue));
}

MusicPlayer::~MusicPlayer() {
    mpDelegate = NULL;
}

void MusicPlayer::setDelegate(MusicPlayerDelegate *delegate) {
    mpDelegate = delegate;
}

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

MusicPlayer::PlayerState MusicPlayer::state() {
    return mState;
}

void MusicPlayer::queue(SongInfo *song, uint32_t index) {
    mDecoder.enablePlayback();

    mState = PLAYING;

    if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {
        xQueueReset(mSongQueue);
        mSongIndex = index;

        //const uint8_t id3Size = 128;
        //char data[id3Size];
        //Storage::read(path, data, id3Size, song->fileSize - id3Size);


        bufferTask->newSongSelected = true;
        xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
        mpDelegate->willStartPlaying(song);
        xQueueSend(mSongQueue, &song, portMAX_DELAY);
    }
}

void MusicPlayer::pause() {
    //mDecoder.disablePlayback();
    mpDelegate->willPause();
    mState = PAUSED;
}

void MusicPlayer::resume() {
    mDecoder.resumePlayback();
    //xSemaphoreGive(mPlaySema);
    mpDelegate->willResume();
    mState = PLAYING;
}

void MusicPlayer::playPrevious() {
    if (!xSemaphoreTake(mPlayMutex, 10)) return;

    uint32_t index = mSongIndex;
    if (index == 0) index = mSongList.size() - 1;
    else            index -= 1;

    queue(&mSongList.at(index), index);
    xSemaphoreGive(mPlayMutex);
}

void MusicPlayer::playNext() {
    if (!xSemaphoreTake(mPlayMutex, 10)) return;

    uint32_t index = mSongIndex;
    if (index == mSongList.size() - 1) index = 0;
    else                               index += 1;

    queue(&mSongList.at(index), index);
    xSemaphoreGive(mPlayMutex);
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

// UITableViewDataSource & UITableViewDelegate Implementation

inline uint32_t MusicPlayer::numberOfItems() const {
    return mSongList.size();
}

inline void MusicPlayer::cellForIndex(UITableViewCell &cell, uint32_t index) {
    SongInfo info = mSongList.at(index);
    cell.setText(info.name);
}

inline void MusicPlayer::didSelectCellAt(UITableViewCell &cell, uint32_t index) {
    SongInfo *song = songAt(index);
    queue(song, index);              // queue song for playback
}

// BufferMusicTask Implementation

bool MusicPlayer::BufferMusicTask::run(void *) {

    const uint32_t bufferSize = STREAM_QUEUE_BUFFER_SIZE;

    while (1) {
        SongInfo *song = NULL;
        if (xQueueReceive(mSongQueue, &song, portMAX_DELAY)) {

            const uint32_t fileSize = song->fileSize;
            const char *path = song->path;

            newSongSelected = false;

            for (uint32_t i = 0; i < fileSize/bufferSize; i++) {
                uint8_t data[bufferSize] = { 0 };

                // if paused wait for player to be resumed to continue sending data...
                // TODO: should use semaphore
                while (player->state() == MusicPlayer::PAUSED) vTaskDelay(1);

                // Terminate buffering if a new song is selected or player is completely stopped...
                if (newSongSelected || player->state() == MusicPlayer::STOPPED) goto ENDSTREAM;

                if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {
                    Storage::read(path, data, bufferSize, i * bufferSize);
                    xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);
                    xQueueSend(mStreamQueue, data, portMAX_DELAY);
                }
                vTaskDelay(15);
            }

            // end of song or user did not manually select a song, play next song....
            player->playNext();

            // new song was selected, or user is skipping song
            ENDSTREAM:;
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
                MP3.buffer(data + (j*buffSize), buffSize);
            }
            vTaskDelay(15);
        }
    }

    return true;
}

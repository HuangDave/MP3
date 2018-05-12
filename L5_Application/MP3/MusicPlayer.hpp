/*
 * MusicPlayer.h
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#ifndef MUSICPLAYER_HPP_
#define MUSICPLAYER_HPP_

#include <MP3/Drivers/VS1053B.hpp>

#include <vector>

#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"

// TODO: remove
#include "storage.hpp"
#include "io.hpp"

class MusicPlayer {

protected:

    typedef struct {
        char *name;
    } SongInfo;

    static std::vector<SongInfo> mSongList;
    static uint32_t mSongCount;

    VS1053B &mDecoder = VS1053B::sharedInstance();

    QueueHandle_t mStreamQueue;
    char *mpCurrentSongName;

    //VS1053B::DecoderState mState;

public:

    MusicPlayer();
    virtual ~MusicPlayer();

    static std::vector<SongInfo> getSongList() { return mSongList; };
    static uint32_t getSongCount()             { return mSongList.size(); };

    void fetchSongs();

    QueueHandle_t getSharedQueue() const { return mStreamQueue; }

    void play(char *songName);
    void pause();

    void incrementVolume();
    void decrementVolume();
};

class BufferMusicTask: public scheduler_task {

protected:

    VS1053B &mDecoder = VS1053B::sharedInstance();
    QueueHandle_t mQueue;

public:

    BufferMusicTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("buffer_song", 1024 * 3, priority), mQueue(queue) { };

    bool run(void *) {
        const uint32_t fileSize = 1024 * 1000 * 11.074; //2.850;
        const uint32_t size = 1024;

        while (1) {
            for (uint32_t i = 0; i < fileSize/size; i++) {
                uint8_t data[size] = { 0 };
                Storage::read("1:rain_320.mp3", data, size, i * size);
                xQueueSend(mQueue, data, portMAX_DELAY);
                vTaskDelay(10);
            }
        }

        return true;
    };
};


class StreamMusicTask: public scheduler_task {

protected:

    VS1053B &mDecoder = VS1053B::sharedInstance();
    QueueHandle_t mQueue;

public:

    StreamMusicTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("stream_song", 1024 * 2, priority), mQueue(queue) { };

    bool run(void *) {
        const uint32_t size = 1024;

        while (1) {
            uint8_t data[size] = { 0 };
            if (xQueueReceive(mQueue, data, portMAX_DELAY)) {
                for (uint32_t j = 0; j < size/32; j++){
                    MP3.buffer(data + (j*32), 32);
                }
            }
        }

        return true;
    };
};

#endif /* MUSICPLAYER_H_ */

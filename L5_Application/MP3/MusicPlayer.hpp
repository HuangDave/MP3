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

class DecodeTask;

typedef struct {
    char *name;
} SongInfo;

class MusicPlayer {

protected:

    typedef enum {
        STOPPED    = (1 << 0),
        PLAYING    = (1 << 1),
        CANCELLING = (1 << 2),
    } PlayerState;

    static std::vector<SongInfo> mSongList;
    static uint32_t mSongCount;

    VS1053B &mDecoder = VS1053B::sharedInstance();

    DecodeTask *mDecodeTask;

    QueueHandle_t mStreamQueue;
    char *mpCurrentSongName;

    PlayerState mState;

public:

    MusicPlayer();
    virtual ~MusicPlayer();

    DecodeTask* getDecodeTask() const;

    static std::vector<SongInfo> getSongList() { return mSongList; };
    static uint32_t getSongCount()             { return mSongList.size(); };

    void fetchSongs();

    void play(char *songName);
    void pause();

    void incrementVolume();
    void decrementVolume();
};

class DecodeTask: public scheduler_task {

protected:

    VS1053B &mDecoder = VS1053B::sharedInstance();
    QueueHandle_t *mQueue;

public:
    DecodeTask(uint8_t priority) : scheduler_task("buffer_song", 2000, priority), mQueue(NULL) { };

    void setQueue(QueueHandle_t *queue) { mQueue = queue; };

    bool run(void *) {
        uint8_t *data = NULL;
        while(xQueueReceive(mQueue, data, portMAX_DELAY)) {
            if (mDecoder.isPlaybackEnabled()) {
                for (uint8_t i = 32; i <= 512; i = i + 32) {
                    mDecoder.buffer(data, VS1053B_BUFFER_SIZE);
                    data += i;
                }
            }
        }
        return true;
    };
};

#endif /* MUSICPLAYER_H_ */

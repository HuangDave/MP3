/*
 * MusicPlayer.h
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#ifndef MUSICPLAYER_HPP_
#define MUSICPLAYER_HPP_

#include <MP3/Drivers/VS1053B.hpp>

#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"

class DecodeTask;

class MusicPlayer {

protected:

    VS1053B &mDecoder = VS1053B::sharedInstance();

    DecodeTask *mDecodeTask;

    QueueHandle_t mStreamQueue;
    char *mpCurrentSongName;

    void buffer(uint8_t songData[32]);

public:

    MusicPlayer();
    virtual ~MusicPlayer();

    DecodeTask* getDecodeTask() const;

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

    void setQueue(QueueHandle_t *queue) { mQueue = queue;     };

    bool run(void *) {
        uint8_t *data = NULL;
        while(xQueueReceive(mQueue, data, portMAX_DELAY)) {
            if (mDecoder.isPlaybackEnabled()) {
                mDecoder.buffer(data, VS1053B_BUFFER_SIZE);
            }
        }
        return true;
    };
};

#endif /* MUSICPLAYER_H_ */

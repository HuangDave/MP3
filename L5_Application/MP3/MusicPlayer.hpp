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

typedef struct {
    char *path;
    char *name;
    uint32_t fileSize;
} SongInfo;

class MusicPlayer {

private:

    class BufferMusicTask;
    class StreamMusicTask;

protected:

    static std::vector<SongInfo> mSongList;
    static uint32_t mSongCount;

    VS1053B &mDecoder = VS1053B::sharedInstance();

    QueueHandle_t mStreamQueue;
    QueueHandle_t mSongQueue;

    char *mpCurrentSongName;

    /// Volume percentage, ranges from 0 to 100.
    uint8_t mVolume;

    /**
     * Set the volume of the decoder.
     * @param percentage Ranges from 0 to 100%.
     */
    inline void setVolume(uint8_t percentage);

public:

    MusicPlayer();
    virtual ~MusicPlayer();

    QueueHandle_t getSharedQueue() const { return mStreamQueue; }

    void play(SongInfo *song);
    void queue(SongInfo *song);
    void pause();

    /// Increment the music player volume by 5%.
    void incrementVolume();
    /// Decrement the music player volume by 5%.
    void decrementVolume();

};

class MusicPlayer::BufferMusicTask final: public scheduler_task {

protected:
    VS1053B &mDecoder = VS1053B::sharedInstance();
    SongInfo *mpCurrentSong;
    QueueHandle_t mSongQueue;
    QueueHandle_t mStreamQueue;

public:
    BufferMusicTask(uint8_t priority, QueueHandle_t songQueue, QueueHandle_t streamQueue) : scheduler_task("buffer_song", 1024 * 3, priority), mSongQueue(songQueue), mStreamQueue(streamQueue) { };
    bool run(void *);
};

class MusicPlayer::StreamMusicTask final: public scheduler_task {

protected:
    VS1053B &mDecoder = VS1053B::sharedInstance();
    QueueHandle_t mStreamQueue;

public:
    StreamMusicTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("stream_song", 1024 * 2, priority), mStreamQueue(queue) { };
    bool run(void *);
};

#endif /* MUSICPLAYER_H_ */

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
    /// Full file path to song.
    char *path;
    /// Formated file name w/o dir prefix and extension type.
    char *name;
    /// total size of file in bytes.
    uint32_t fileSize;
} SongInfo;

class MusicPlayer {

private:

    class BufferMusicTask;
    class StreamMusicTask;

protected:

    typedef enum {
        STOPPED = 0,
        PLAYING,
        PAUSED
    } PlayerState;

    static MusicPlayer *instance;

    static std::vector<SongInfo> mSongList;
    static uint32_t mSongCount;

    VS1053B &mDecoder = VS1053B::sharedInstance();

    QueueHandle_t mStreamQueue;
    QueueHandle_t mSongQueue;

    /// Semaphore to puase or resume playback.
    SemaphoreHandle_t mPlaySema;

    char *mpCurrentSongName;

    /// Volume percentage, ranges from 0 to 100.
    uint8_t mVolume;

    MusicPlayer();

    /**
     * Set the volume of the decoder.
     *
     * @param percentage Ranges from 0 to 100%.
     */
    inline void setVolume(uint8_t percentage);

public:

    static MusicPlayer& sharedInstance();

    virtual ~MusicPlayer();

    void queue(SongInfo *song);
    void pause();
    void resume();

    void playNext();
    void playPrevious();

    /// Increment the music player volume by 5%.
    void incrementVolume();
    /// Decrement the music player volume by 5%.
    void decrementVolume();
};

/**
 * Task to read and buffer data of a song from the SD Card and then queue the data to be ready for decoding.
 */
class MusicPlayer::BufferMusicTask final: public scheduler_task {

protected:
    VS1053B &mDecoder = VS1053B::sharedInstance();
    //SongInfo *mpCurrentSong;
    QueueHandle_t mSongQueue;
    QueueHandle_t mStreamQueue;

public:
    BufferMusicTask(uint8_t priority, QueueHandle_t songQueue, QueueHandle_t streamQueue) : scheduler_task("buffer_song", 1024 * 3, priority), mSongQueue(songQueue), mStreamQueue(streamQueue) { };
    bool run(void *);
};

/**
 * Task to dequeue buffered data from BufferMusicTask for decoding.
 */
class MusicPlayer::StreamMusicTask final: public scheduler_task {

protected:
    VS1053B &mDecoder = VS1053B::sharedInstance();
    QueueHandle_t mStreamQueue;

public:
    StreamMusicTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("stream_song", 1024 * 2, priority), mStreamQueue(queue) { };
    bool run(void *);
};

#endif /* MUSICPLAYER_H_ */

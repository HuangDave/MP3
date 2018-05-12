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

class MusicPlayer {

private:

    class BufferMusicTask;
    class StreamMusicTask;

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

class MusicPlayer::BufferMusicTask final: public scheduler_task {

protected:
    VS1053B &mDecoder = VS1053B::sharedInstance();
    QueueHandle_t mQueue;

public:
    BufferMusicTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("buffer_song", 1024 * 3, priority), mQueue(queue) { };
    bool run(void *);
};

class MusicPlayer::StreamMusicTask final: public scheduler_task {

protected:
    VS1053B &mDecoder = VS1053B::sharedInstance();
    QueueHandle_t mQueue;

public:
    StreamMusicTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("stream_song", 1024 * 2, priority), mQueue(queue) { };
    bool run(void *);
};

#endif /* MUSICPLAYER_H_ */

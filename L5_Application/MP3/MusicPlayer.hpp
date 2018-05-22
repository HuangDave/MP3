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
#include <memory>

#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"

#include <MP3/UI/UITableView.hpp>

/**
 * SongInfo
 *
 * @param path      Full directory filepath of the song.
 * @param name      Formated name of the song (w/o directory prefix and .mp3 extension)
 * @param fileSize  Size in bytes of the mp3 file.
 */
typedef struct {
    /// Full file path to song.
    char *path;
    /// Formated file name w/o dir prefix and extension type.
    char *name;
    /// total size of file in bytes.
    uint32_t fileSize;
} SongInfo;

/**
 * MusicPlayerDelegate
 *
 * Used to communicate between the player and UI to update the NowPlayingView.
 */
class MusicPlayerDelegate {
public:
    virtual void willStartPlaying(SongInfo *song) = 0;
    virtual void willPause() = 0;
    virtual void willResume() = 0;
    virtual void willStop() = 0;
};

/**
 * THe MusicPlayer class handles the buffering of the song data to the VS1053B audio decoder.
 *
 * On initialization, all mp3 files are fetched from the SD Card and stored into mSongList.
 *
 * The function queue(SongInfo *song, uint32_t index) is called queue a selected song for the BufferMusicTask
 * to start fetching data for the selected song.
 *
 * When the state of the player is changed, the MusicPlayerDelegate (NowPlayingView) is notified of the change in state to update
 * the view and display relevant information to the user.
 *
 * Additionally, the MusicPlayer also conforms and implments UITableViewDataSource and UITableViewDelegate to populate the song menu.
 */
class MusicPlayer: protected virtual UITableViewDataSource, UITableViewDelegate {

private:

    class BufferMusicTask;
    class StreamMusicTask;
    class FetchMusicTask;

public:

    typedef enum {
        STOPPED = 0,
        PLAYING,
        PAUSED
    } PlayerState;

    static MusicPlayer& sharedInstance();

    virtual ~MusicPlayer();

    void setDelegate(MusicPlayerDelegate *delegate);

    SongInfo* songAt(uint32_t idx) { return &mSongList.at(idx); }

    /// @return Returns the current state of the player.
    PlayerState state();

    /**
     * Queue a song for playback.
     *
     * @param song  Pointer to the SongInfo of the song to queue.
     * @param index Index of the song in mSongList.
     */
    void queue(SongInfo *song, uint32_t index);

    /// Pause the music player and also pause decoding of the current song. resume() is called to resume the player and decoding.
    void pause();
    void resume();

    /**
     * If the player is currently playing a song, the player will go back to the previous song.
     * If the beginning of the song list is reach, the player will loop back and play the last
     * song on the song list.
     *
     * On reset, when playPrevious() is called, the play will play the last song in the song list.
     */
    void playPrevious();

    /**
     * If the player is currently playing a song, the player will skip to the next song.
     * If the end of the song list is reached, the player will start at the first song.
     *
     * On reset, if playNext() is called, the player will play the song following the first song.
     */
    void playNext();

    /// Increment the music player volume by 5%.
    void incrementVolume();
    /// Decrement the music player volume by 5%.
    void decrementVolume();

protected:

    typedef enum {
        PREVIOUS_TRACK = 0,
        NEXT_TRACK,
    } QueueOption;

    static MusicPlayer *instance;

    std::vector<SongInfo> mSongList;

    VS1053B &mDecoder = VS1053B::sharedInstance();

    /// Delegate for receiving any updates for when the player's state is changed.
    MusicPlayerDelegate *mpDelegate;

    /// Task to read and buffer the current song data from SD Card.
    BufferMusicTask *bufferTask;

    /// Used to communicate with the buffer task to start buffering a selected song.
    QueueHandle_t mSongQueue;

    // TODO: remove
    SemaphoreHandle_t mPlayMutex;

    /// Current state of music player.
    PlayerState mState;

    /// Volume percentage, ranges from 0 to 100.
    uint8_t mVolume;

    /// Index of the current song that is being decoded and played.
    uint32_t mSongIndex;

    MusicPlayer();

    /// Fetch a list of all mp3 files from the SD Card.
    void fetchSongs();

    /**
     * Set the volume of the decoder.
     *
     * @param percentage Ranges from 0 to 100%.
     */
    inline void setVolume(uint8_t percentage);

    // UITableViewDataSource & UITableViewDelegate

    /// Returns the number of songs in mSongList.
    virtual inline uint32_t numberOfItems() const final;

    /// Updates a table view cell with song info based on the index.
    virtual inline void cellForIndex(UITableViewCell &cell, uint32_t index) final;

    virtual inline void didSelectCellAt(UITableViewCell &cell, uint32_t index) final;

};

class MusicPlayer::FetchMusicTask final: public scheduler_task {
protected:

public:
    FetchMusicTask(uint8_t priority) : scheduler_task("fetch_music", 1024, priority) { };
    bool run(void *);
};

/**
 * Task to read and buffer data of a song from the SD Card and then queue the data to be ready for decoding.
 */
class MusicPlayer::BufferMusicTask final: public scheduler_task {

protected:
    QueueHandle_t mSongQueue;
    QueueHandle_t mStreamQueue;

public:

    MusicPlayer *player;
    bool newSongSelected;

    BufferMusicTask(uint8_t priority, QueueHandle_t songQueue, QueueHandle_t streamQueue) : scheduler_task("buffer_song", 1024 * 3, priority), mSongQueue(songQueue), mStreamQueue(streamQueue) { };
    bool run(void *);
};

/**
 * Task to dequeue buffered data from BufferMusicTask and send to VS1053B audio decoder for decoding.
 */
class MusicPlayer::StreamMusicTask final: public scheduler_task {

protected:
    QueueHandle_t mStreamQueue;

public:
    StreamMusicTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("stream_song", 1024 * 2, priority), mStreamQueue(queue) { };
    bool run(void *);
};

#endif /* MUSICPLAYER_H_ */

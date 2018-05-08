/*
 * MusicPlayer.cpp
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#include <MP3/MusicPlayer.hpp>

MusicPlayer::MusicPlayer() {
    //mDecoder = VS1053B::sharedInstance();
    mDecodeTask = new DecodeTask(PRIORITY_MEDIUM);
    mStreamQueue = xQueueCreate(2, sizeof(uint8_t));
    mpCurrentSongName = NULL;
}

MusicPlayer::~MusicPlayer() {
    // TODO Auto-generated destructor stub
}

DecodeTask* MusicPlayer::getDecodeTask() const { return mDecodeTask; };

void MusicPlayer::play(char *songName) {
    mDecoder.enablePlayback();

    mpCurrentSongName = songName;

    while (mDecoder.isPlaybackEnabled()) {
        // fetch audio file from SD Card...
        uint8_t *data = new uint8_t[VS1053B_BUFFER_SIZE];

        buffer(data);
    }
}

void MusicPlayer::pause() {
    mDecoder.disablePlayback();
}

void MusicPlayer::buffer(uint8_t songData[32]) {
    if (xQueueSend(mStreamQueue, &songData, portMAX_DELAY)) {
        //decodeSong();
    }
}

void MusicPlayer::incrementVolume() {

}
void MusicPlayer::decrementVolume() {

}

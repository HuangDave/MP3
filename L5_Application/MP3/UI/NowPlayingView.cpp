/*
 * NowPlayingView.cpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#include <MP3/UI/NowPlayingView.hpp>
#include <string.h>
#include <MP3/Drivers/ST7735.hpp>

const uint16_t playIcon[] = {
    0b0111111111111111,
    0b0011111111111110,
    0b0001111111111100,
    0b0000111111111000,
    0b0000011111110000,
    0b0000001111100000,
    0b0000000111000000,
    0b0000000010000000
};

const uint16_t pausedIcon[] = {
    0b0000111000111000,
    0b0000111000111000,
    0b0000111000111000,
    0b0000111000111000,
    0b0000111000111000,
    0b0000111000111000,
    0b0000111000111000,
    0b0000111000111000
};

const uint16_t stopIcon[] = {
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000
};

NowPlayingView::NowPlayingView(Frame frame) : UIView(frame) {
    mState = MusicPlayer::STOPPED;
    mpSongName = NULL;
}

NowPlayingView::~NowPlayingView() {
    mpSongName = NULL;
}

void NowPlayingView::setSongName(char* const name) {
    mpSongName = name;
    reDraw();
}

void NowPlayingView::reDraw() {
    UIView::reDraw();

    // Frame for drawing play, pause, or stop icon...
    Frame iconFrame = Frame { mFrame.x + 5, mFrame.y + 6, 8, 16 };

    switch (mState) {
        case MusicPlayer::STOPPED: {
            LCDDisplay.drawBitmap(iconFrame, stopIcon, BLACK_COLOR, mBackgroundColor);
        } break;

        case MusicPlayer::PLAYING: {
            LCDDisplay.drawBitmap(iconFrame, playIcon, BLACK_COLOR, mBackgroundColor);

            uint8_t len = strlen(mpSongName);
            if (len > 20) len = 20;

            char str[len];
            strncpy(str, mpSongName, len);
            for (uint8_t i = len - 3; i < len; i++)
                str[i] = '.';

            // TODO: move to UILabel Class
            for (uint8_t i = 0; i < len; i++) {
                const uint8_t padding = mFrame.x + 20;
                const uint8_t charSpacing = (i * 1);
                const uint8_t charPos = (i * 5);

                const uint8_t x = padding + charSpacing + charPos;
                const uint8_t y = mFrame.y + 10;

                const uint8_t *bitmap = Font[int(mpSongName[i])];

                LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);
            }
        } break;

        case MusicPlayer::PAUSED: {
            LCDDisplay.drawBitmap(iconFrame, pausedIcon, BLACK_COLOR, mBackgroundColor);
        } break;
    }
}

// MusicPlayerDelegate Implementation

void NowPlayingView::willStartPlaying(SongInfo *song) {
    mState = MusicPlayer::PLAYING;
    setSongName(song->name);
}

void NowPlayingView::willPause() {
    mState = MusicPlayer::PAUSED;
}

void NowPlayingView::willResume() {
    mState = MusicPlayer::PLAYING;
}

void NowPlayingView::willStop() {
    mState = MusicPlayer::STOPPED;
}

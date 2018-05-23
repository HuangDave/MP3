/*
 * NowPlayingView.cpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#include <MP3/UI/NowPlayingView.hpp>
#include <string.h>
#include <MP3/Drivers/ST7735.hpp>
#include "MP3/MP3File.hpp"

const uint16_t playIcon[] = {
    0b0000000000000000,
    0b0001111111111100,
    0b0000111111111000,
    0b0000011111110000,
    0b0000001111100000,
    0b0000000111000000,
    0b0000000010000000,
    0b0000000000000000
};

const uint16_t pausedIcon[] = {
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000000000000000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000000000000000,
};

const uint16_t stopIcon[] = {
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000000000000000
};

NowPlayingView::NowPlayingView(Frame frame) : View(frame) {
    mState = MusicPlayer::STOPPED;
    mpSong = NULL;
}

NowPlayingView::~NowPlayingView() {
    mpSong = NULL;
}

void NowPlayingView::reDraw() {
    if (mIsDirty) View::reDraw();

    // Frame for drawing play, pause, or stop icon...
    Frame iconFrame = Frame { mFrame.x + 5, mFrame.y + 6, 8, 16 };

    switch (mState) {
        case MusicPlayer::STOPPED: {
            LCDDisplay.drawBitmap(iconFrame, stopIcon, BLACK_COLOR, mBackgroundColor);
        } break;

        case MusicPlayer::PLAYING: {
            LCDDisplay.drawBitmap(iconFrame, playIcon, BLACK_COLOR, mBackgroundColor);

            uint8_t len = strlen(mpSong->getTitle());
            if (len > 20) len = 20;

            char str[len];
            if (len > 21) {
                len = 21;
                strncpy(str, mpSong->getTitle(), len);
                for (uint8_t i = len-3; i < len; i++)
                    str[i] = '.';

            } else {
                strncpy(str, mpSong->getTitle(), len);
            }

            for (uint8_t i = 0; i < len; i++) {
                const uint8_t padding = mFrame.x + 20;
                const uint8_t charSpacing = (i * 1);
                const uint8_t charPos = (i * 5);

                const uint8_t x = padding + charSpacing + charPos;
                const uint8_t y = mFrame.y + 5;

                const uint8_t *bitmap = Font[int(str[i])];

                LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);
            }


            for (uint8_t i = 0; i < strlen(mpSong->getArtist()); i++) {
                const uint8_t padding = mFrame.x + 20;
                const uint8_t charSpacing = (i * 1);
                const uint8_t charPos = (i * 5);

                const uint8_t x = padding + charSpacing + charPos;
                const uint8_t y = mFrame.y + 15;

                const uint8_t *bitmap = Font[int(mpSong->getArtist()[i])];

                LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);
            }

        } break;

        case MusicPlayer::PAUSED: {
            LCDDisplay.drawBitmap(iconFrame, pausedIcon, BLACK_COLOR, mBackgroundColor);
        } break;
    }

    mIsDirty = false;
}

// MusicPlayerDelegate Implementation

void NowPlayingView::willStartPlaying(MP3File *song) {
    mState = MusicPlayer::PLAYING;
    mpSong = song;
    mIsDirty = true;
    reDraw();
}

void NowPlayingView::willPause() {
    mState = MusicPlayer::PAUSED;
    reDraw();
}

void NowPlayingView::willResume() {
    mState = MusicPlayer::PLAYING;
    reDraw();
}

void NowPlayingView::willStop() {
    mState = MusicPlayer::STOPPED;
    reDraw();
}

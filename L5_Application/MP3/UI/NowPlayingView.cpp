/*
 * NowPlayingView.cpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#include <MP3/UI/NowPlayingView.hpp>
#include <string.h>
#include <MP3/Drivers/ST7735.hpp>

// TODO: remove printf
#include <stdio.h>

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

const uint16_t pauseIcon[] = {
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
    mpSongName = NULL;
}

NowPlayingView::~NowPlayingView() {
    mpSongName = NULL;
}

void NowPlayingView::reDraw() {
    UIView::reDraw();

    // TODO: fix eyeballed values

    Frame playIconFrame = Frame { mFrame.x + 5, mFrame.y + 6, 8, 16 };
    LCDDisplay.drawBitmap(playIconFrame, playIcon, BLACK_COLOR, mBackgroundColor);

    // TODO: move to UILabel Class
    uint8_t len = strlen(mpSongName);
    for (uint8_t i = 0; i < len; i++) {
        const uint8_t padding = mFrame.x + 20;
        const uint8_t charSpacing = (i * 1);
        const uint8_t charPos = (i * 5);

        const uint8_t x = padding + charSpacing + charPos;
        const uint8_t y = mFrame.y + 10;

        const uint8_t *bitmap = Font[int(mpSongName[i])];

        LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);
    }
}

// UITableViewDelegate Implementation

// TODO: move to UserInterface
inline void NowPlayingView::didSelectCellAt(UITableViewCell &cell, uint32_t index) {
    printf("now playing: %s", cell.getText());
    //SongInfo info = mSongList[index];

    // TODO: queue songname for playback
    //MusicPlayer::play(info.fullName);

    mpSongName = cell.getText();
    reDraw();
}

/*
 * UserInterface.cpp
 *
 *  Created on: May 8, 2018
 *      Author: huang
 */

#include <MP3/UserInterface.hpp>

#include <stdio.h>
#include <string.h>

// TODO: should use external interrupts
#include "io.hpp"

#include <L1/LabGPIO.hpp>
#include <MP3/Drivers/ST7735.hpp>
#include <MP3/MusicPlayer.hpp>
#include <MP3/UI/NowPlayingView.hpp>

UserInterface::~UserInterface() {
    delete mpSongMenu;   mpSongMenu   = NULL;
    delete mpNowPlaying; mpNowPlaying = NULL;
    mpSubviews.empty();
}

// Member Functions

bool UserInterface::init() {

    // Initialize song menu
    const uint8_t kMenuHeight = 100;
    const uint8_t kMenuRowHeight = 10;
    const uint8_t kNumRows = 10;

    mpSongMenu = new UITableView(Frame { 0, 0, SCREEN_WIDTH, kMenuHeight });
    mpSongMenu->setDelegate((UITableViewDelegate *) this);
    mpSongMenu->setDataSource((UITableViewDataSource *) &(MusicPlayer::sharedInstance()));
    mpSongMenu->setRowHeight(kMenuRowHeight);
    mpSongMenu->setNumberOfRows(kNumRows);
    addSubview(mpSongMenu);

    // Initialize Now Player view
    const uint8_t kNowPlayingHeight = SCREEN_HEIGHT - kMenuHeight;

    mpNowPlaying = new NowPlayingView(Frame {0, kMenuHeight, SCREEN_WIDTH, kNowPlayingHeight});
    mpNowPlaying->setBackgroundColor(BLUE_COLOR);
    addSubview(mpNowPlaying);

    return true;
}

bool UserInterface::run(void *) {

    updateViews();

    // TODO: remap GPIO buttons...

    LabGPIO *bPlay     = new LabGPIO(0, 29);
    LabGPIO *bPlayPrev = new LabGPIO(0, 30);
    LabGPIO *bPlayNext = new LabGPIO(1, 19);

    LabGPIO *bMenuSel  = new LabGPIO(1, 20);//(1, 20),     // menu select
    LabGPIO *bMenuUp   = new LabGPIO(1, 22);//(1, 22),     // menu cursor up
    LabGPIO *bMenuDown = new LabGPIO(1, 23);//(1, 23),     // menu cursor down

    LabGPIO *bVolUp    = new LabGPIO(1, 28);
    LabGPIO *bVolDown  = new LabGPIO(1, 29);

    MusicPlayer &player = MusicPlayer::sharedInstance();

    while (1) {

        if      (bPlay)                 { player.state() == MusicPlayer::PLAYING ? player.pause() : player.resume(); } // TODO: check player state to pause or resume...
        else if (bPlayPrev->getLevel()) { player.playPrevious(); }
        else if (bPlayNext->getLevel()) { player.playNext(); }

        else if (bMenuSel->getLevel())  { mpSongMenu->selectCurrentRow(); }
        else if (bMenuUp->getLevel())   { mpSongMenu->moveCursor(UITableView::DIRECTION_UP); }
        else if (bMenuDown->getLevel()) { mpSongMenu->moveCursor(UITableView::DIRECTION_DOWN); }

        else if (bVolUp->getLevel())    { player.incrementVolume(); }
        else if (bVolDown->getLevel())  { player.decrementVolume(); }

        //else if (buttons[BACK]->getLevel())      { mpSongMenu->selectCurrentRow(); }
        vTaskDelay(100);
    }

    return true;
}

void UserInterface::addSubview(UIView *view) {
    mpSubviews.push_back(view);
}

void UserInterface::updateViews() {
    for (UIView *view : mpSubviews) {
        view->reDraw();
    }
}

// UITableViewDelegate Implementation

inline void UserInterface::didSelectCellAt(UITableViewCell &cell, uint32_t index) {

    MusicPlayer &player = MusicPlayer::sharedInstance();
    SongInfo *song = player.songAt(index);

    // update NowPlayingView to display current song...
    (*mpNowPlaying).setSongName(song->name);

    // queue song for playback
    player.queue(song);
    //player.queue(player.songAt(index+1));
}

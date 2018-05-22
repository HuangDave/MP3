/*
 * UserInterface.cpp
 *
 *  Created on: May 8, 2018
 *      Author: huang
 */

#include <MP3/UserInterface.hpp>

#include <stdio.h>
#include <string.h>

#include <L1/LabGPIO.hpp>
#include <MP3/Drivers/ST7735.hpp>
#include <MP3/MusicPlayer.hpp>
#include <MP3/UI/NowPlayingView.hpp>
#include <MP3/UI/TableView.hpp>

typedef enum {
    PLAY      = 0,
    PLAY_PREV,
    PLAY_NEXT,
    VOL_UP,
    VOL_DOWN,

    MENU_SEL,
    MENU_UP,
    MENU_DOWN,

    BUTTON_COUNT
} Button_Map;

UserInterface::~UserInterface() {
    delete mpSongMenu;   mpSongMenu   = NULL;
    delete mpNowPlaying; mpNowPlaying = NULL;
    mSubviews.empty();
}

// Member Functions

bool UserInterface::init() {

    MusicPlayer &player = MusicPlayer::sharedInstance();

    // TODO: initialize main menu...

    // Initialize song menu...
    const uint8_t kMenuHeight    = 100;
    const uint8_t kMenuRowHeight = 10;
    const uint8_t kNumRows       = 10;

    mpSongMenu = new TableView(Frame { 0, 0, SCREEN_WIDTH, kMenuHeight });
    mpSongMenu->setDelegate((TableViewDelegate *) &player);
    mpSongMenu->setDataSource((TableViewDataSource *) &player);
    mpSongMenu->setRowHeight(kMenuRowHeight);
    mpSongMenu->setNumberOfRows(kNumRows);
    addSubview(mpSongMenu);

    // Initialize Now Player view...
    const uint8_t kNowPlayingHeight = SCREEN_HEIGHT - kMenuHeight;

    mpNowPlaying = new NowPlayingView(Frame {0, kMenuHeight, SCREEN_WIDTH, kNowPlayingHeight});
    mpNowPlaying->setBackgroundColor(BLUE_COLOR);
    player.setDelegate((MusicPlayerDelegate *) mpNowPlaying);
    addSubview(mpNowPlaying);

    // initialize GPIO buttons...
    // TODO: fix this
    //mpButtons            = new LabGPIO[BUTTON_COUNT];
    /*
    mpButtons[PLAY]      = LabGPIO(0, 29);
    mpButtons[PLAY_PREV] = LabGPIO(1, 15); // TODO: need to remap should be 1, 19
    mpButtons[PLAY_NEXT] = LabGPIO(1, 14); // TODO: need to remap should be 0, 30
    mpButtons[VOL_UP]    = LabGPIO(1, 28);
    mpButtons[VOL_DOWN]  = LabGPIO(1, 29);
    mpButtons[MENU_SEL]  = LabGPIO(1, 23);
    mpButtons[MENU_UP]   = LabGPIO(1,  9); // TODO: need to remap should be 1, 20
    mpButtons[MENU_DOWN] = LabGPIO(1, 22);
     */
    return true;
}

bool UserInterface::run(void *) {

    //vTaskDelay(10);

    updateViews();

    // TODO: remap GPIO buttons...

    LabGPIO *bPlay     = new LabGPIO(1, 10); // (0, 29)
    LabGPIO *bPlayPrev = new LabGPIO(1, 14);//(0, 30);
    LabGPIO *bPlayNext = new LabGPIO(1, 15);//(1, 19);

    LabGPIO *bMenuSel  = new LabGPIO(1,  9);//(1, 20);//(1, 20),     // menu select
    LabGPIO *bMenuUp   = new LabGPIO(1, 22);//(1, 22),     // menu cursor up
    LabGPIO *bMenuDown = new LabGPIO(1, 23);     // menu cursor down

    LabGPIO *bVolUp    = new LabGPIO(1, 28);
    LabGPIO *bVolDown  = new LabGPIO(1, 29);

    MusicPlayer &player = MusicPlayer::sharedInstance();

    while (1) {

        if      (bPlay->getLevel())     { player.state() == MusicPlayer::PLAYING ? player.pause() : player.resume(); vTaskDelay(500); }
        else if (bPlayPrev->getLevel()) { player.playPrevious(); vTaskDelay(500); }
        else if (bPlayNext->getLevel()) { player.playNext();     vTaskDelay(500); }

        else if (bMenuSel->getLevel())  { mpSongMenu->selectCurrentRow(); }
        else if (bMenuUp->getLevel())   { mpSongMenu->moveCursor(TableView::DIRECTION_UP); }
        else if (bMenuDown->getLevel()) { mpSongMenu->moveCursor(TableView::DIRECTION_DOWN); }

        else if (bVolUp->getLevel())    { player.incrementVolume(); }
        else if (bVolDown->getLevel())  { player.decrementVolume(); }

        //else if (buttons[BACK]->getLevel())      { mpSongMenu->selectCurrentRow(); }

        vTaskDelay(50);
    }

    return true;
}

void UserInterface::addSubview(View *view) {
    mSubviews.push_back(view);
}

void UserInterface::updateViews() {
    for (View *view : mSubviews) {
        view->reDraw();
    }
}

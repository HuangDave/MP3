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

#include <MP3/Drivers/ST7735.hpp>
#include <MP3/MusicPlayer.hpp>
#include <MP3/UI/SongMenu.hpp>
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

    mpSongMenu = new SongMenu(Frame { 0, 0, SCREEN_WIDTH, kMenuHeight });
    addSubview(mpSongMenu);

    // Initialize Now Player view
    const uint8_t kNowPlayingHeight = SCREEN_HEIGHT - kMenuHeight;

    mpNowPlaying = new NowPlayingView(Frame {0, kMenuHeight, SCREEN_WIDTH, kNowPlayingHeight});
    (*mpNowPlaying).setBackgroundColor(BLUE_COLOR);
    (*mpSongMenu).setDelegate((UITableViewDelegate *)mpNowPlaying);
    addSubview(mpNowPlaying);

    return true;
}

bool UserInterface::run(void *) {

    updateViews();

    // TODO: change to external interrupts

    while (1) {
        if      (SW.getSwitch(1)) { mpSongMenu->moveCursor(UITableView::DIRECTION_UP);   }
        else if (SW.getSwitch(2)) { mpSongMenu->moveCursor(UITableView::DIRECTION_DOWN); }
        else if (SW.getSwitch(3)) { mpSongMenu->selectCurrentRow(); vTaskDelay(1000);    }
        vTaskDelay(1);
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

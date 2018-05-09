/*
 * UserInterface.cpp
 *
 *  Created on: May 8, 2018
 *      Author: huang
 */

#include <MP3/UserInterface.hpp>

#include <stdio.h>
#include <string.h>
#include "io.hpp"

#include <MP3/Drivers/ST7735.hpp>
#include <MP3/MusicPlayer.hpp>

//#include <MP3/UI/UITableView.hpp>

UserInterface::UserInterface(uint8_t priority) : scheduler_task("ui", 2048, priority)  {

}

UserInterface::~UserInterface() {
    delete mpSongMenu;   mpSongMenu   = NULL;
    delete mpNowPlaying; mpNowPlaying = NULL;
    mpSubviews.empty();
}

// Member Functions

bool UserInterface::init() {

    // Initialize song menu
    const uint8_t kMenuHeight = 100;
    //const uint8_t kRowHeight  = 10;
    //const uint8_t kNumRows    = 10;

    mpSongMenu = new UITableView(Frame { 0, 0, SCREEN_WIDTH, kMenuHeight });
    (*mpSongMenu).setDataSource((UITableViewDataSource *)this);
    (*mpSongMenu).setDelegate((UITableViewDelegate *)this);
    addSubview(mpSongMenu);

    // Initialize Now Player view
    const uint8_t kNowPlayingHeight = SCREEN_HEIGHT - kMenuHeight;

    mpNowPlaying = new UIView(Frame {0, kMenuHeight, SCREEN_WIDTH, kNowPlayingHeight});
    (*mpNowPlaying).setBackgroundColor(BLUE_COLOR);
    addSubview(mpNowPlaying);

    updateViews();
    return true;
}

bool UserInterface::run(void *) {

    // TODO: change to external interrupts

    while (1) {
        if      (SW.getSwitch(1)) { mpSongMenu->moveCursor(UITableView::DIRECTION_UP);   }
        else if (SW.getSwitch(2)) { mpSongMenu->moveCursor(UITableView::DIRECTION_DOWN); }
        else if (SW.getSwitch(3)) { mpSongMenu->selectCurrentRow(); vTaskDelay(1000);    }
        vTaskDelay(1);
    }
}

void UserInterface::addSubview(UIView *view) {
    mpSubviews.push_back(view);
}

void UserInterface::updateViews() {
    for (UIView *view : mpSubviews) {
        view->reDraw();
    }
}

// UITableView Datasource & Delegate

uint32_t UserInterface::numberOfItems() const {
    return MusicPlayer::getSongCount();
}

uint8_t UserInterface::numberOfRows() const {
    return 10;
}

uint8_t UserInterface::rowHeight() const {
    return 10;
}

void UserInterface::cellForIndex(UITableViewCell &cell, uint32_t index) {
    SongInfo info = MusicPlayer::getSongList()[index];

    const uint8_t newLen = strlen(info.name) - 3;
    char fmtName[newLen];
    strncpy(fmtName, info.name, newLen);
    //cell.setText(fmtName, newLen);

    cell.setText(info.name, strlen(info.name));
}

void UserInterface::didSelectCellAt(UITableViewCell &cell, uint32_t index) {
    printf("now playing: %s", cell.getText());
}

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
//#include <MP3/UI/UITableView.hpp>

UserInterface::UserInterface(uint8_t priority) : scheduler_task("ui", 2048, priority)  {
    // Initialize song menu
    const uint8_t kMenuHeight = 100;
    //const uint8_t kRowHeight  = 10;
    //const uint8_t kNumRows    = 10;

    mpSongMenu = new UITableView(Frame { 0, 0, SCREEN_WIDTH, kMenuHeight });

    //mpSongMenu->setItemCount(songCount);
    //mpSongMenu->attachCellSelectHandler(&(UserInterface::userDidSelectSong));
    //mpSongMenu->attachCellUpdateHandler(&(UserInterface::updateSongeItem));
    addSubview(mpSongMenu);

    // Initialize Now Player view
    const uint8_t kNowPlayingHeight = SCREEN_HEIGHT - kMenuHeight;

    mpNowPlaying = new UIView(Frame {0, kMenuHeight, SCREEN_WIDTH, kNowPlayingHeight});
    mpNowPlaying->setBackgroundColor(BLUE_COLOR);
    addSubview(mpNowPlaying);
}

UserInterface::~UserInterface() {
    delete mpSongMenu;   mpSongMenu   = NULL;
    delete mpNowPlaying; mpNowPlaying = NULL;
    mpSubviews.empty();
}

// Member Functions

bool UserInterface::init() {
    (*mpSongMenu).setDataSource((UITableViewDataSource *)this);
    (*mpSongMenu).setDelegate((UITableViewDelegate *)this);
    updateViews();
    return true;
}

bool UserInterface::run(void *) {

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
    return 0;
}

uint8_t UserInterface::numberOfRows() const {
    return 10;
}

uint8_t UserInterface::rowHeight() const {
    return 10;
}

void UserInterface::cellForIndex(UITableViewCell &cell, uint32_t index) {

}

void UserInterface::didSelectCellAt(UITableViewCell &cell, uint32_t index) {

}

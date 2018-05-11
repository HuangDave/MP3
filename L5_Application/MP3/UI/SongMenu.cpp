/*
 * SongMenu.cpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#include <MP3/UI/SongMenu.hpp>
#include <string.h>
#include <MP3/MusicPlayer.hpp>

#include <stdio.h>

SongMenu::SongMenu(Frame frame) : UIView(frame) {

    // Initialize song menu
    const uint8_t kMenuHeight = 100;

    mpSongMenu = new UITableView(Frame { 0, 0, SCREEN_WIDTH, kMenuHeight });
    (*mpSongMenu).setDelegate((UITableViewDelegate *)this);


}

SongMenu::~SongMenu() {

}


// UITableViewDelegate

uint32_t SongMenu::numberOfItems() const {
    return MusicPlayer::getSongCount();
}

void SongMenu::cellForIndex(UITableViewCell &cell, uint32_t index) {
    SongInfo info = MusicPlayer::getSongList()[index];

    const uint8_t newLen = strlen(info.name) - 3;
    char fmtName[newLen];
    strncpy(fmtName, info.name, newLen);
    //cell.setText(fmtName, newLen);

    cell.setText(info.name, strlen(info.name));
}

void SongMenu::didSelectCellAt(UITableViewCell &cell, uint32_t index) {
    printf("now playing: %s", cell.getText());
}

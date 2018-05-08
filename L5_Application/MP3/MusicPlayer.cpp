/*
 * MusicPlayer.cpp
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#include <MP3/MusicPlayer.hpp>

MusicPlayer::MusicPlayer()
{
    const uint8_t kRowHeight = 10;
    const uint8_t kNumRows  = 10;

    tableView = new UITableView(Frame { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
    tableView->setRowHeight(kRowHeight);
    tableView->setMininmumRows(kNumRows);
    tableView->attachCellUpdateHandler(&updateSongItem);
    tableView->update();
}

MusicPlayer::~MusicPlayer()
{
    // TODO Auto-generated destructor stub
}

void MusicPlayer::refresh() {

}

void MusicPlayer::updateSongItem(UITableViewCell &cell, uint32_t index) {

}

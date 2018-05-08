/*
 * MusicPlayer.h
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#ifndef MUSICPLAYER_HPP_
#define MUSICPLAYER_HPP_

#include "MP3/UITableView.hpp"

class MusicPlayer {

protected:

    // song selection menu
    UITableView *tableView;

    void updateSongItem(UITableViewCell &cell, uint32_t index);

    void refresh();

public:

    MusicPlayer();
    virtual ~MusicPlayer();
};

#endif /* MUSICPLAYER_H_ */

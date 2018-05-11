/*
 * SongMenu.hpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#ifndef SONGMENU_HPP_
#define SONGMENU_HPP_

#include <MP3/UI/UITableView.hpp>
#include <vector>

class SongMenu final: public UITableView, protected virtual UITableViewDataSource {

protected:

    typedef struct {
        char *fmtName;
        char *fullName;
    } SongInfo;

    /// Vector array of list of song names retreived from SD Card.
    std::vector<SongInfo> mSongList;

    /**
     * Fetch list of songs from SD Card
     */
    void fetchSongs();

    // UITableViewDataSource

    virtual uint32_t numberOfItems() const final;
    virtual void cellForIndex(UITableViewCell &cell, uint32_t index) final;

public:

    SongMenu(Frame frame);
    virtual ~SongMenu();
};

#endif /* SONGMENU_HPP_ */

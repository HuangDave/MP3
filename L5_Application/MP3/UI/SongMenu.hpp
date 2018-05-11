/*
 * SongMenu.hpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#ifndef SONGMENU_HPP_
#define SONGMENU_HPP_

#include <MP3/UI/UITableView.hpp>

class SongMenu: public UIView, protected virtual UITableViewDelegate {

protected:

    UITableView *mpSongMenu;

    // UITableViewDelegate

    virtual uint32_t numberOfItems() const final;
    virtual void cellForIndex(UITableViewCell &cell, uint32_t index) final;
    virtual void didSelectCellAt(UITableViewCell &cell, uint32_t index) final;

public:

    SongMenu(Frame frame);

    virtual ~SongMenu();
};

#endif /* SONGMENU_HPP_ */

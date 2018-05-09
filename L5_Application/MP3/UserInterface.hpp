/*
 * UserInterface.hpp
 *
 *  Created on: May 8, 2018
 *      Author: huang
 */

#ifndef USERINTERFACE_HPP_
#define USERINTERFACE_HPP_

#include <vector>

#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"

#include <MP3/UI/UITableView.hpp>

class UserInterface: public scheduler_task, virtual protected UITableViewDataSource, virtual protected UITableViewDelegate {

protected:

    UITableView *mpSongMenu;
    UIView *mpNowPlaying;

    std::vector<UIView *> mpSubviews;

    // UITableView Datasource & Delegate

    virtual uint32_t numberOfItems() const final;
    virtual uint8_t numberOfRows() const final;
    virtual uint8_t rowHeight() const final;
    virtual void cellForIndex(UITableViewCell &cell, uint32_t index) final;

    virtual void didSelectCellAt(UITableViewCell &cell, uint32_t index) final;

public:

    UserInterface(uint8_t priority);
    virtual ~UserInterface();

    bool init();
    bool run(void *);

    void addSubview(UIView *view);
    void updateViews();

};

#endif /* USERINTERFACE_HPP_ */

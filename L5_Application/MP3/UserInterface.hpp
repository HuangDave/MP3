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

class UIView;
class UITableView;
class UITableViewCell;

class UserInterface: public scheduler_task {

protected:

    UITableView *mpSongMenu;
    UIView *mpNowPlaying;

    std::vector<UIView *> mpSubviews;

public:

    UserInterface(uint8_t priority);
    virtual ~UserInterface();

    bool init();
    bool run(void *);

    UITableView& songMenu() { return *mpSongMenu; };

    void addSubview(UIView *view);

    void updateViews();
};

#endif /* USERINTERFACE_HPP_ */

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

class LabGPIO;
class UIView;
class NowPlayingView;
class UITableView;

class UserInterface: public scheduler_task {

protected:

    std::vector<UIView *> mSubviews;
    LabGPIO *mpButtons;

    UITableView *mpSongMenu;
    NowPlayingView *mpNowPlaying;

public:

    UserInterface(uint8_t priority) : scheduler_task("ui", 1024, priority) { };
    virtual ~UserInterface();

    bool init();
    bool run(void *);

    void addSubview(UIView *view);
    void updateViews();

};

#endif /* USERINTERFACE_HPP_ */

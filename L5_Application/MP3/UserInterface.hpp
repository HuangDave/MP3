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
class View;
class NowPlayingView;
class TableView;

class UserInterface: public scheduler_task {

protected:

    std::vector<View *> mSubviews;
    LabGPIO *mpButtons;

    TableView *mpSongMenu;
    NowPlayingView *mpNowPlaying;

public:

    UserInterface(uint8_t priority) : scheduler_task("ui", 1024, priority) { };
    virtual ~UserInterface();

    bool init();
    bool run(void *);

    void addSubview(View *view);
    void updateViews();

};

#endif /* USERINTERFACE_HPP_ */

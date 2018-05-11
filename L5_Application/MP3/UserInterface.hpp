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
class SongMenu;
class NowPlayingView;

class UserInterface: public scheduler_task {

protected:

    SongMenu *mpSongMenu;
    NowPlayingView *mpNowPlaying;

    std::vector<UIView *> mpSubviews;

public:

    UserInterface(uint8_t priority) : scheduler_task("ui", 1024, priority) { };
    virtual ~UserInterface();

    bool init();
    bool run(void *);

    void addSubview(UIView *view);
    void updateViews();

};

#endif /* USERINTERFACE_HPP_ */

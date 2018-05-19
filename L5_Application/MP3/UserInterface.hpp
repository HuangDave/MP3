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

class NowPlayingView;
class LabGPIO;

class UserInterface: public scheduler_task, protected virtual UITableViewDelegate {

protected:

    std::vector<UIView *> mSubviews;
    LabGPIO *mpButtons;

    UITableView *mpSongMenu;
    NowPlayingView *mpNowPlaying;

    // UITableViewDelegate

    virtual inline void didSelectCellAt(UITableViewCell &cell, uint32_t index) final;

public:

    UserInterface(uint8_t priority) : scheduler_task("ui", 1024, priority) { };
    virtual ~UserInterface();

    bool init();
    bool run(void *);

    void addSubview(UIView *view);
    void updateViews();

};

#endif /* USERINTERFACE_HPP_ */

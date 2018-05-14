/*
 * NowPlayingView.hpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#ifndef NOWPLAYINGVIEW_HPP_
#define NOWPLAYINGVIEW_HPP_

#include <MP3/UI/UITableView.hpp>

class NowPlayingView: public UIView, protected virtual UITableViewDelegate {

protected:

    char *mpSongName;

    // UITableViewDelegate
    
    virtual inline void didSelectCellAt(UITableViewCell &cell, uint32_t index) final;

public:
    NowPlayingView(Frame frame);
    virtual ~NowPlayingView();

    virtual void reDraw() override;

};

#endif /* NOWPLAYINGVIEW_HPP_ */

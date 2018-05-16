/*
 * NowPlayingView.hpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#ifndef NOWPLAYINGVIEW_HPP_
#define NOWPLAYINGVIEW_HPP_

#include <MP3/UI/UIView.hpp>

class NowPlayingView: public UIView {

protected:

    char *mpSongName;

public:

    NowPlayingView(Frame frame);
    virtual ~NowPlayingView();

    void setSongName(char* const name);

    virtual void reDraw() override;
};

#endif /* NOWPLAYINGVIEW_HPP_ */

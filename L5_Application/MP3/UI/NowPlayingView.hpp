/*
 * NowPlayingView.hpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#ifndef NOWPLAYINGVIEW_HPP_
#define NOWPLAYINGVIEW_HPP_

#include <MP3/UI/View.hpp>
#include <MP3/MusicPlayer.hpp>

class NowPlayingView: public View, protected virtual MusicPlayerDelegate {

protected:

    char *mpSongName;

    MusicPlayer::PlayerState mState;

    // MusicPlayerDelegate

    virtual void willStartPlaying(SongInfo *song) final;
    virtual void willPause() final;
    virtual void willResume() final;
    virtual void willStop() final;

public:

    NowPlayingView(Frame frame);
    virtual ~NowPlayingView();

    /**
     * Updates the view to display the name of the current song.
     *
     * @param name Name of song to display.
     */
    void setSongName(char* const name);

    /**
     * Redraw the view.
     */
    virtual void reDraw() override;
};

#endif /* NOWPLAYINGVIEW_HPP_ */

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

class MP3File;

/**
 * The NowPlayingView is used to display the song title and artist of the song that is currently selected.
 * When the user selects, puases, or resumes a song, the view is notified through the MusicPlayerDelegate to
 * perform any updates to the view.
 */
class NowPlayingView: public View, protected virtual MusicPlayerDelegate {

protected:

    /// Current song that is being played and displayed.
    MP3File *mpSong;

    MusicPlayer::PlayerState mState;

    // MusicPlayerDelegate

    /**
     * Update the view to display the info of the selected song.
     *
     * @param song Song that is currently selected.
     */
    virtual void willStartPlaying(MP3File *song) final;
    /// Update the indicator to show the puased icon.
    virtual void willPause() final;
    /// Update the indicator to show the playing icon.
    virtual void willResume() final;
    /// Update the indicator to show the stop icon.
    virtual void willStop() final;

public:

    NowPlayingView(Frame frame);
    virtual ~NowPlayingView();

    /**
     * Redraw the view.
     */
    virtual void reDraw() override;
};

#endif /* NOWPLAYINGVIEW_HPP_ */

/*
 * UIView.hpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#ifndef UIVIEW_HPP_
#define UIVIEW_HPP_

#include "../gfx.h"

/**
 * The UIView class the base class for displaying views on the LCD Display.
 *
 * The frame of the view determines the origin of the view as well as the width and height it occupies on the display.
 * The view is drawn when the reDraw() functions are called.
 */
class UIView {

public:

    UIView(Frame frame) : mFrame(frame), mBackgroundColor(WHITE_COLOR) { };
    virtual ~UIView();

    void setFrame(Frame frame);
    void setOrigin(Point2D origin);
    void setSize(Size2D size);
    void setBackgroundColor(Color c);

    virtual void reDraw();
    virtual void reDrawWithBackground(Color *color);

protected:

    Frame mFrame;
    Color mBackgroundColor;
};

#endif /* UIVIEW_HPP_ */

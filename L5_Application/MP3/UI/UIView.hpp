/*
 * UIView.hpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#ifndef UIVIEW_HPP_
#define UIVIEW_HPP_

#include "../gfx.h"
#include <stddef.h>

class UIView {

public:

    UIView(Frame frame);
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

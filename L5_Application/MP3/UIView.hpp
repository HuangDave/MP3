/*
 * UIView.hpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#ifndef UIVIEW_HPP_
#define UIVIEW_HPP_

#include "gfx.h"
#include "ST7735.hpp"

class UIView {

public:

    UIView(Frame frame);
    virtual ~UIView();

    void setFrame(Frame frame);
    void setOrigin(Point2D origin);
    void setSize(Size2D size);
    void setBackgroundColor(Color c);

    // void setBorderColor(Color c);
    // void setSelected(bool selected);

     void reDraw();

protected:

    Frame mFrame;
    Color mBackgroundColor;
    //Color mBorderColor;
    //bool  mSelected;

};

#endif /* UIVIEW_HPP_ */

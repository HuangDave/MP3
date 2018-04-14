/*
 * UIView.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: huang
 */

#include <MP3/UIView.hpp>
#include "ST7735.hpp"

Layer::Layer(Size2D size) : mSize(size) {
    //Color *x = new Color(size.width);
    //Color *y = new Color(size.height);
}

UIView::UIView(Frame frame) : mFrame(frame) {
    mBackgroundColor = WHITE_COLOR;
    mSelected = false;
    //mBorderColor = BLACK_COLOR;
}

UIView::~UIView() {

}

void UIView::setFrame(Frame frame)       { mFrame = frame; }
void UIView::setOrigin(Point2D origin)   { mFrame.origin = origin; }
void UIView::setSize(Size2D size)        { mFrame.size = size; }
void UIView::setBackgroundColor(Color c) { mBackgroundColor = c; }
// void UIView::setBorderColor(Color c)     { mBorderColor = c; }

void UIView::draw() {
    Point2D p;
    p.x = mFrame.origin.x + mFrame.size.width;
    p.y = mFrame.origin.y + mFrame.size.height;
    LCDDisplay.setAddrWindow(mFrame.origin, p);
}

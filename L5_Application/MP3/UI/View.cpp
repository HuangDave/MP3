/*
 * View.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: huang
 */

#include "View.hpp"

#include <stddef.h>
#include <MP3/Drivers/ST7735.hpp>

View::View(Frame frame) {
    mFrame = frame;
    mBackgroundColor = WHITE_COLOR;
    mIsDirty = true;
}

View::~View() { }

void View::setFrame(Frame frame) {
    mFrame = frame;
}

void View::setOrigin(Point2D origin) {
    mFrame.origin = origin;
}
void View::setSize(Size2D size) {
    mFrame.size = size;
}

void View::setBackgroundColor(Color c) {
    mBackgroundColor = c;
}

void View::reDraw() {
    reDrawWithBackground(NULL);
}

void View::reDrawWithBackground(Color *color) {
    if (mIsDirty) {
        LCDDisplay.fillRect(mFrame, color == NULL ? mBackgroundColor : *color);
        mIsDirty = false;
    }
}

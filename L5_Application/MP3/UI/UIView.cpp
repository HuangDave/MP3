/*
 * UIView.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: huang
 */

#include "UIView.hpp"

#include <stddef.h>
#include <MP3/Drivers/ST7735.hpp>

UIView::~UIView() { }

void UIView::setFrame(Frame frame)              { mFrame = frame; }

void UIView::setOrigin(Point2D origin)          { mFrame.origin = origin; }
void UIView::setSize(Size2D size)               { mFrame.size = size; }
void UIView::setBackgroundColor(Color c)        { mBackgroundColor = c; }

void UIView::reDraw()                           { reDrawWithBackground(NULL); }
void UIView::reDrawWithBackground(Color *color) { LCDDisplay.fillRect(mFrame, color == NULL ? mBackgroundColor : *color); }

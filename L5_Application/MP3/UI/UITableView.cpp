/*
 * UITableView.cpp
 *
 *  Created on: May 6, 2018
 *      Author: huang
 */

#include "UITableView.hpp"
#include <stdio.h>
#include <string.h>
#include "utilities.h"
#include <MP3/Drivers/ST7735.hpp>

// ---------------------------------------------------------- //
//                        UITableView                         //
// ---------------------------------------------------------- //

UITableView::UITableView(Frame frame) : UIView(frame) {
    mpCells = NULL;
    mpDataSource = NULL;
    mpDelegate = NULL;
    mRows = 0;
    mRowHeight = 0;
    mCursorPos = 0;
    mIndexStart = 0;
    mIndexEnd = 0;
    mItemCount = 0;
}

UITableView::~UITableView() {
    mpDataSource = NULL;
    mpDelegate = NULL;

    delete mpCells;
    mpCells = NULL;
}

void UITableView::updateTableIfNeeded() {

    if (mpCells == NULL && mRows > 0 || mItemCount != (*mpDataSource).numberOfItems()) {

        mItemCount = (*mpDataSource).numberOfItems();

        mpCells = new UITableViewCell[mRows];
        for (uint8_t i = 0; i < mRows; i++) {
            uint8_t y = i * mRowHeight + (!!i * 1);
            mpCells[i] = UITableViewCell(Frame { 0, y, mFrame.width, mRowHeight });
        }

        highlightCellAt(0); // highlight the first cell

        // reset indexing
        mCursorPos = 0;
        mIndexStart = 0;
        mIndexEnd = mRows - 1;
    }
}

void UITableView::reDraw() {

    updateTableIfNeeded();

    for (uint8_t row = 0; row < mRows; row++) {
        uint32_t index = mIndexStart + row;
        if (index >= mItemCount) break;
        (*mpDataSource).cellForIndex(cellForRow(row), index);
        reDraw(row);
    }
}

void UITableView::reDraw(uint8_t row)                                    { cellForRow(row).reDraw(); }
void UITableView::setDataSource(UITableViewDataSource* const dataSource) { mpDataSource = dataSource; }
void UITableView::setDelegate(UITableViewDelegate* const delegate)       { mpDelegate = delegate; }
void UITableView::setNumberOfRows(uint8_t rows)                          { mRows = rows; }
void UITableView::setRowHeight(uint8_t height)                           { mRowHeight = height; }
void UITableView::selectCurrentRow()                                     { (*mpDelegate).didSelectCellAt(cellForRow(mCursorPos), mIndexStart + mCursorPos); }
UITableViewCell& UITableView::cellForRow(uint8_t row)                    { return mpCells[row]; }

void UITableView::highlightCellAt(uint8_t row) {
    cellForRow(row).setHighlighted(true);
    reDraw(row);
}

void UITableView::unhighlightCellAt(uint8_t row) {
    cellForRow(row).setHighlighted(false);
    reDraw(row);
}

void UITableView::moveCursor(CursorDirection direction) {
    const uint32_t prevPos = mCursorPos;

    switch (direction) {
        
        case DIRECTION_UP: {
            // clamp cursor to top
            if (mCursorPos != 0) { // cursor is not already at the top
                mCursorPos--;
                unhighlightCellAt(prevPos);
                highlightCellAt(mCursorPos);

            } else {
                if( mIndexStart == 0) return;   // reached the top
                mIndexStart--;
                mIndexEnd--;
                reDraw();
            }
        } break;

        case DIRECTION_DOWN: {
            if (mCursorPos == mItemCount - 1) return;

            // clamp cursor to bottom
            if (mCursorPos < mRows-1) { // cursor is not already at the bottom
                mCursorPos++;
                unhighlightCellAt(prevPos);
                highlightCellAt(mCursorPos);

            } else {
                if (mIndexEnd == mItemCount - 1) return; // reached the end
                mIndexStart++;
                mIndexEnd++;
                reDraw();
            }
        } break;
    }
}

// ---------------------------------------------------------- //
//                      UITableViewCell                       //
// ---------------------------------------------------------- //

UITableViewCell::UITableViewCell() : UIView(Frame{0,0,0,0}) {
    init();
}

UITableViewCell::UITableViewCell(Frame frame) : UIView(frame) {
    init();
}

void UITableViewCell::init() {
    mpText = NULL;
    mTextLen = 0;

    mHighlighted = false;
    mHighlightedColor = BLACK_COLOR;
}

void UITableViewCell::setText(char *text, uint32_t len) {
    mpText = text;
    mTextLen = len;

    reDraw();
}

void UITableViewCell::setHighlighted(bool highlighted) { mHighlighted = highlighted; }
void UITableViewCell::setHighlightedColor(Color color) { mHighlightedColor = color;  }

void UITableViewCell::reDraw() {
    // use highlighted color if cell is highlighted
    //UIView::reDrawWithBackground( mHighlighted ? &mHighlightedColor : &mBackgroundColor );

    uint8_t selIcon[] = {
            0b11111110,
            0b01111100,
            0b00111000,
            0b00010000
    };

    uint8_t unselIcon[] = { 0, 0, 0, 0 };

    Frame selFrame = Frame { mFrame.x + 2, mFrame.y, 4, 8 };

    uint8_t len = mTextLen > 15 ? 18 : mTextLen;

    char str[18];
    strncpy(str, mpText, 15);
    for (uint8_t i = 15; i < 18; i++)
        str[i] = '.';

    if   (mHighlighted) LCDDisplay.drawBitmap(selFrame, selIcon,   BLACK_COLOR, mBackgroundColor);
    else                LCDDisplay.drawBitmap(selFrame, unselIcon, BLACK_COLOR, mBackgroundColor);

    // TODO: move to UILabel Class
    for (uint8_t i = 0; i < len; i++) {
        const uint8_t padding = mFrame.x + 8;
        const uint8_t charSpacing = (i * 1);
        const uint8_t charPos = (i * 5);

        const uint8_t x = padding + charSpacing + charPos;
        const uint8_t y = mFrame.y + 1;

        const uint8_t *bitmap = Font[int(str[i])];

        //if   (mHighlighted) LCDDisplay.drawFont(Point2D{x, y}, bitmap, WHITE_COLOR, mHighlightedColor);
        //else                LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);

        LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);

        //delay_ms(2);
    }
}

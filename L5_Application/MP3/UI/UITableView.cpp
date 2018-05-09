/*
 * UITableView.cpp
 *
 *  Created on: May 6, 2018
 *      Author: huang
 */

#include "UITableView.hpp"
#include <stdio.h>
#include <string.h>

#include <MP3/Drivers/ST7735.hpp>

// ---------------------------------------------------------- //
//                        UITableView                         //
// ---------------------------------------------------------- //

UITableView::UITableView(Frame frame) : UIView(frame) {
    mpCells = NULL;
    mRows = 0;
    mRowHeight = 0;
    mCursorPos = 0;
    mIndexStart = 0;
    mIndexEnd = 0;
    mItemCount = 0;
    mInvalidated = true;
    mDividerColor = BLACK_COLOR;
}

UITableView::~UITableView() {
    mDataSource = NULL;
    mDelegate   = NULL;

    delete mpCells;
    mpCells = NULL;
}

void UITableView::updateTableIfNeeded() {

    if ( (mpCells == NULL && mRows > 0) || mInvalidated) {

        mRows      = (*mDataSource).numberOfRows();
        mItemCount = (*mDataSource).numberOfItems();
        mRowHeight = (*mDataSource).rowHeight();

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
        (*mDataSource).cellForIndex(cellForRow(row), index);
        //mCellUpdateHandler(cellForRow(row), index);
        reDraw(row);
    }
}

void UITableView::reDraw(uint8_t row)                              { cellForRow(row).reDraw(); }

void UITableView::setDataSource(UITableViewDataSource* const dataSource) { mDataSource = dataSource; }
void UITableView::setDelegate(UITableViewDelegate* const delegate)       { mDelegate = delegate; }

void UITableView::setDividerColor(Color color)                     { mDividerColor = color;                    }

void UITableView::selectCurrentRow()                               { (*mDelegate).didSelectCellAt(cellForRow(mCursorPos), mIndexStart + mCursorPos); }

UITableViewCell& UITableView::cellForRow(uint8_t row)              { return mpCells[row]; }

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
                //update();
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
                //update();
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
    UIView::reDrawWithBackground( mHighlighted ? &mHighlightedColor : &mBackgroundColor );

    uint8_t len = mTextLen > 15 ? 18 : mTextLen;

    char str[18];
    strncpy(str, mpText, 15);
    for (uint8_t i = 15; i < 18; i++)
        str[i] = '.';

    for (uint8_t i = 0; i < len; i++) {
        const uint8_t padding = mFrame.x + 4;
        const uint8_t charSpacing = (i * 1);
        const uint8_t charPos = (i * 5);

        const uint8_t x = padding + charSpacing + charPos;
        const uint8_t y = mFrame.y + 1;

        const uint8_t *bitmap = Font[int(str[i])];

        if   (mHighlighted) LCDDisplay.drawFont(Point2D{x, y}, bitmap, WHITE_COLOR, mHighlightedColor);
        else                LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);
    }
}

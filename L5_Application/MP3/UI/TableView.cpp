/*
 * TableView.cpp
 *
 *  Created on: May 6, 2018
 *      Author: huang
 */

#include "TableView.hpp"
#include <stdio.h>
#include <string.h>
#include "utilities.h"
#include <MP3/Drivers/ST7735.hpp>

// ---------------------------------------------------------- //
//                        TableView                           //
// ---------------------------------------------------------- //

TableView::TableView(Frame frame) : View(frame) {
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

TableView::~TableView() {
    mpDataSource = NULL;
    mpDelegate = NULL;

    delete mpCells;
    mpCells = NULL;
}

void TableView::updateTableIfNeeded() {

    if ( (mpCells == NULL && mRows > 0) || mItemCount != (*mpDataSource).numberOfItems()) {

        mItemCount = (*mpDataSource).numberOfItems();

        mpCells = new TableViewCell[mRows];
        for (uint8_t i = 0; i < mRows; i++) {
            uint8_t y = i * mRowHeight + (!!i * 1);
            mpCells[i] = TableViewCell(Frame { 0, y, mFrame.width, mRowHeight });
        }

        highlightCellAt(0); // highlight the first cell

        // reset indexing
        mCursorPos = 0;
        mIndexStart = 0;
        mIndexEnd = mRows - 1;
    }
}

void TableView::reDraw() {

    updateTableIfNeeded();

    for (uint8_t row = 0; row < mRows; row++) {
        uint32_t index = mIndexStart + row;
        if (index >= mItemCount) break;
        (*mpDataSource).cellForIndex(cellForRow(row), index);
        reDraw(row);
    }
}

void TableView::reDraw(uint8_t row)                                    { cellForRow(row).reDraw(); }
void TableView::setDataSource(TableViewDataSource* const dataSource) { mpDataSource = dataSource; }
void TableView::setDelegate(TableViewDelegate* const delegate)       { mpDelegate = delegate; }
void TableView::setNumberOfRows(uint8_t rows)                          { mRows = rows; }
void TableView::setRowHeight(uint8_t height)                           { mRowHeight = height; }
void TableView::selectCurrentRow()                                     { (*mpDelegate).didSelectCellAt(cellForRow(mCursorPos), mIndexStart + mCursorPos); }
TableViewCell& TableView::cellForRow(uint8_t row)                    { return mpCells[row]; }

void TableView::highlightCellAt(uint8_t row) {
    cellForRow(row).setHighlighted(true);
    reDraw(row);
}

void TableView::unhighlightCellAt(uint8_t row) {
    cellForRow(row).setHighlighted(false);
    reDraw(row);
}

void TableView::moveCursor(CursorDirection direction) {
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
//                      TableViewCell                       //
// ---------------------------------------------------------- //

TableViewCell::TableViewCell() : View(Frame{0,0,0,0}) {
    init();
}

TableViewCell::TableViewCell(Frame frame) : View(frame) {
    init();
}

void TableViewCell::init() {
    mpText = NULL;

    mHighlighted = false;
    mHighlightedColor = BLACK_COLOR;
}

void TableViewCell::setText(char *text) {
    mpText = text;
    reDraw();
}

void TableViewCell::setHighlighted(bool highlighted) { mHighlighted = highlighted; }
void TableViewCell::setHighlightedColor(Color color) { mHighlightedColor = color;  }

void TableViewCell::reDraw() {
    // use highlighted color if cell is highlighted
    //View::reDrawWithBackground( mHighlighted ? &mHighlightedColor : &mBackgroundColor );

    uint8_t selIcon[] = {
            0b11111110,
            0b01111100,
            0b00111000,
            0b00010000
    };

    uint8_t unselIcon[] = { 0, 0, 0, 0 };

    Frame selFrame = Frame { mFrame.x + 2, mFrame.y, 4, 8 };

    uint8_t len = strlen(mpText);
    char str[len];

    if (len > 21) {
        len = 21;
        strncpy(str, mpText, len);
        for (uint8_t i = len-3; i < len; i++)
            str[i] = '.';

    } else {
        strncpy(str, mpText, len);
    }

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

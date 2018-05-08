/*
 * UITableView.cpp
 *
 *  Created on: May 6, 2018
 *      Author: huang
 */

#include <MP3/UITableView.hpp>

// ---------------------------------------------------------- //
//                      UITableViewCell                       //
// ---------------------------------------------------------- //

UITableViewCell::UITableViewCell() : UIView(Frame{0,0,0,0}) {

}

void UITableViewCell::setText(char *text, uint32_t len) {
    mpText = text;
    mTextLen = len;

    reDraw();
}

void UITableViewCell::setHighlighted(bool highlighted) { mHighlighted = highlighted; }
void UITableViewCell::setHighlightedColor(Color color) { mHighlightedColor = color;  }

void UITableViewCell::reDraw() {
    UIView::reDraw( mHighlighted ? &mHighlightedColor : &mBackgroundColor );       // use highlighted color if cell is highlighted

    for (uint8_t i = 0; i < mTextLen; i++) {
        const uint8_t padding = mFrame.x + 4;
        const uint8_t charSpacing = (i * 1);
        const uint8_t charPos = (i * 5);

        const uint8_t x = padding + charSpacing + charPos;
        const uint8_t y = mFrame.y + 1;

        const uint8_t *bitmap = Font[int(mpText[i])];

        if (mHighlighted) LCDDisplay.drawFont(Point2D{x, y}, bitmap, WHITE_COLOR, mHighlightedColor);
        else             LCDDisplay.drawFont(Point2D{x, y}, bitmap, BLACK_COLOR, mBackgroundColor);
    }
}

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

    mDividerColor = BLACK_COLOR;
}

UITableView::~UITableView() {
    delete mpCells;
    mpCells = NULL;
}

void UITableView::reDraw() {
    for (uint8_t row = 0; row < mRows; row++) {
        cellForRow(row).reDraw();

        // draw divider
        uint8_t y = row * mRowHeight;
        Frame dividerFrame = Frame { 0, y, SCREEN_WIDTH, 1 };
        LCDDisplay.fillRect(dividerFrame, BLACK_COLOR);

    }
}

void UITableView::update(uint8_t row)                            { cellForRow(row).reDraw(); }
void UITableView::setMininmumRows(uint8_t rows)                  { mRows = rows; }
void UITableView::setRowHeight(uint8_t height)                   { mRowHeight = height; }
void UITableView::setDividerColor(Color color)                   { mDividerColor = color; }
void UITableView::setItemCount(uint32_t count)                   { mItemCount = count; }
void UITableView::attachCellUpdateHandler(UpdateHandler handler) { mCellUpdateHandler = handler; }
UITableViewCell& UITableView::cellForRow(uint8_t row)            { return mpCells[row]; }

void UITableView::highlightCellAt(uint8_t row) {
    cellForRow(row).setHighlighted(false);
    update(row);
}

void UITableView::unhighlightCellAt(uint8_t row) {
    cellForRow(row).setHighlighted(true);
    update(row);
}

void UITableView::cursorDidMoveUp() {
    uint32_t prevPos = mCursorPos;
    // clamp cursor to top
    if (mCursorPos != 0) { // cursor is not already at the top
        mCursorPos--;
        unhighlightCellAt(prevPos);
        highlightCellAt(mCursorPos);
    } else {
        mIndexStart--;
        mIndexEnd--;
        reDraw();
    }
}

void UITableView::cursorDidMoveDown() {
    uint32_t prevPos = mCursorPos;
    // clamp cursor to bottom
    if (mCursorPos < mRows) { // cursor is not already at the bottom
        mCursorPos++;
        unhighlightCellAt(prevPos);
        highlightCellAt(mCursorPos);
    } else {
        mIndexStart++;
        mIndexEnd++;
        reDraw();
    }
}

void UITableView::update() {
    if (mpCells == NULL && mRows > 0) {
        mpCells = new UITableViewCell[mRows];
        for (uint8_t i = 0; i < mRows; i++) {
            uint8_t y = i * mRowHeight + 1;
            mpCells[i] = UITableViewCell(Frame { 0, y, mFrame.width, mRowHeight });
            //mpCells[i].setBackgroundColor(RED_COLOR);
        }
    }

    for (uint8_t row = 0; row < mRows; row++) {
        mCellUpdateHandler(cellForRow(row), mIndexStart + mCursorPos);
    }
    reDraw();
}

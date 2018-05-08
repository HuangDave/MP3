/*
 * UITableView.hpp
 *
 *  Created on: May 6, 2018
 *      Author: huang
 */

#ifndef UITABLEVIEW_HPP_
#define UITABLEVIEW_HPP_

#include "UIView.hpp"

class UITableViewCell;

class UITableView: public UIView {

public:

    typedef void (*UpdateHandler)(UITableViewCell &cell, uint32_t index);
    typedef void (*SelectHandler)(uint32_t index);

    typedef enum {
        DIRECTION_UP   = 0,
        DIRECTION_DOWN ,
    } CursorDirection;

    UITableView(Frame frame);
    virtual ~UITableView();

    void reDraw() override;
    void reDraw(uint8_t row);

    void updateTableIfNeeded();

    void setMininmumRows(uint8_t rows);
    void setRowHeight(uint8_t height);
    void setDividerColor(Color color);
    void setItemCount(uint32_t count);
    void selectCurrentRow();

    void attachCellUpdateHandler(UpdateHandler handler);
    void attachCellSelectHandler(SelectHandler handler);

    void highlightCellAt(uint8_t row);
    void unhighlightCellAt(uint8_t row);

    void moveCursor(CursorDirection direction);

    void cursorDidMoveUp();

protected:

    /// Array of all reusable cells.
    UITableViewCell *mpCells;

    UpdateHandler mCellUpdateHandler;
    SelectHandler mCellSelectHandler;

    /// Total number of reusable rows in table view.
    uint8_t mRows;
    /// Height of each table view rows
    uint8_t mRowHeight;

    Color mDividerColor;

    /// total number of items
    uint32_t mItemCount;

    /// current highlighted row, should be between { 0, mRows - 1 }
    uint8_t mCursorPos;

    // current index range of items that are displayed
    uint8_t mIndexStart;
    uint8_t mIndexEnd;

    bool mInvalidated;

    UITableViewCell& cellForRow(uint8_t row);
};

class UITableViewCell: public UIView {

protected:

    char *mpText;
    uint32_t mTextLen;

    bool mHighlighted;
    Color mHighlightedColor;

    void init();

public:

    UITableViewCell();
    UITableViewCell(Frame frame);

    void setText(char *text, uint32_t len);
    void setHighlighted(bool highlighted);
    void setHighlightedColor(Color color);

    void reDraw() override;
};

#endif /* UITABLEVIEW_HPP_ */

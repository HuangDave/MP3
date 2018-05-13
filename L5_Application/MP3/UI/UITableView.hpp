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

class UITableViewDataSource {
public:
    virtual uint32_t numberOfItems() const = 0;
    virtual void cellForIndex(UITableViewCell &cell, uint32_t index) = 0;
};

class UITableViewDelegate {
public:
    virtual void didSelectCellAt(UITableViewCell &cell, uint32_t index) = 0;
};

class UITableView: public UIView {

public:

    typedef enum {
        DIRECTION_UP   = 0,
        DIRECTION_DOWN ,
    } CursorDirection;

    UITableView(Frame frame);
    virtual ~UITableView();

    void setDataSource(UITableViewDataSource* const dataSource);
    void setDelegate(UITableViewDelegate* const delegate);

    void reDraw() override;

    void updateTableIfNeeded();

    void setNumberOfRows(uint8_t rows);
    void setRowHeight(uint8_t height);

    void selectCurrentRow();

    void highlightCellAt(uint8_t row);
    void unhighlightCellAt(uint8_t row);

    void moveCursor(CursorDirection direction);

    void cursorDidMoveUp();

protected:

    UITableViewDataSource *mpDataSource;
    UITableViewDelegate *mpDelegate;

    /// Array of all reusable cells.
    UITableViewCell *mpCells;

    /// Total number of reusable rows in table view.
    uint8_t mRows;
    /// Height of each table view rows
    uint8_t mRowHeight;

    /// total number of items
    uint32_t mItemCount;

    /// current highlighted row, should be between { 0, mRows - 1 }
    uint8_t mCursorPos;

    // current index range of items that are displayed
    uint8_t mIndexStart;
    uint8_t mIndexEnd;

    UITableViewCell& cellForRow(uint8_t row);
    void reDraw(uint8_t row);
};

class UITableViewCell: public UIView {

protected:

    char *mpText;
    uint32_t mTextLen;
    bool mHighlighted;
    Color mHighlightedColor;

    // Common Init
    void init();

public:

    UITableViewCell();
    UITableViewCell(Frame frame);

    void setText(char *text, uint32_t len);
    char* getText() const { return mpText; };

    void setHighlighted(bool highlighted);
    void setHighlightedColor(Color color);

    void reDraw() override;
};

#endif /* UITABLEVIEW_HPP_ */

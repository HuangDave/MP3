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

    /**
     * @return Returns the total number of items that is to be displayed.
     */
    virtual inline uint32_t numberOfItems() const = 0;

    /**
     * Used to populate the table view.
     *
     * @param cell  Cell to edit.
     * @param index Index of the item that is displayed by the cell.
     */
    virtual inline void cellForIndex(UITableViewCell &cell, uint32_t index) = 0;
};

class UITableViewDelegate {
public:

    /**
     * Called when a cell in the table view becomes selected.
     *
     * @param cell  Dereferenced UITableViewCell that was selected.
     * @param index Index of the cell that was selected.
     */
    virtual inline void didSelectCellAt(UITableViewCell &cell, uint32_t index) = 0;
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

    /**
     * Initializes reusable cells if needed or if the item count or row count is changed.
     */
    void updateTableIfNeeded();

    /**
     * Set number of reusable rows to display.
     *
     * @param rows Number of rows.
     */
    void setNumberOfRows(uint8_t rows);

    /**
     * Set the minimum row height of each row.
     * 
     * @param height Row height.
     */
    void setRowHeight(uint8_t height);

    /**
     * Select the row at the current cursor position.
     */
    void selectCurrentRow();

    void moveCursor(CursorDirection direction);

protected:

    UITableViewDataSource *mpDataSource;
    UITableViewDelegate *mpDelegate;

    /// Array of all reusable cells.
    UITableViewCell *mpCells;

    /// Total number of reusable rows in table view.
    uint8_t mRows;

    /// Height of each row.
    uint8_t mRowHeight;

    /// total number of items.
    uint32_t mItemCount;

    /// current highlighted row, should be between { 0, mRows - 1 }
    uint8_t mCursorPos;

    /// current index range of items that are displayed
    uint8_t mIndexStart;
    uint8_t mIndexEnd;

    UITableViewCell& cellForRow(uint8_t row);

    inline void highlightCellAt(uint8_t row);
    inline void unhighlightCellAt(uint8_t row);

    void reDraw(uint8_t row);
};

class UITableViewCell: public UIView {

protected:

    char *mpText;
    uint32_t mTextLen;

    /// True if the cells is currently highlighted.
    bool mHighlighted;

    /// Background color of highlighted cell.
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

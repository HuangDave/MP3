/*
 * View.hpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#ifndef VIEW_HPP_
#define VIEW_HPP_

#include "../gfx.h"

/**
 * The View class the base class for displaying views on the LCD Display.
 *
 * The frame of the view determines the origin of the view as well as the width and height it occupies on the display.
 * The view is drawn when the reDraw() functions are called.
 */
class View {

public:

    View(Frame frame);
    virtual ~View();

    void setFrame(Frame frame);
    void setOrigin(Point2D origin);
    void setSize(Size2D size);
    void setBackgroundColor(Color c);

    /// Set the view as dirty and completely redraw the view.
    virtual void draw();

    /// Redraw the view and only update parts of the view that needs updating.
    virtual void reDraw();

    /// Redraw the view with a different background color.
    virtual void reDrawWithBackground(Color *color);

protected:

    Frame mFrame;
    Color mBackgroundColor;

    bool mIsDirty;
};

#endif /* VIEW_HPP_ */

/*
 * Tetrominos.hpp
 *
 *  Created on: May 14, 2018
 *      Author: huang
 */

#ifndef TETROMINOS_HPP_
#define TETROMINOS_HPP_

#include <MP3/gfx.h>

class Tetrominos {

public:

    typedef enum {
        I_SHAPE = 0,
        O_SHAPE,
        T_SHAPE,
        S_SHAPE,
        Z_SHAPE,
        J_SHAPE,
        L_SHAPE
    } Shape;

    typedef enum {
        UP    = 0,
        LEFT  = 1,
        RIGHT = 2,
        DOWN  = 3
    } Orientation;


    Tetrominos();
    virtual ~Tetrominos();

    virtual void rotate();

    virtual void reDraw();

protected:

    Point2D mPosition;

    Orientation mOrientation;

    inline uint8_t* getBitmap(Shape shape, Orientation orientation) const;
};

#endif /* TETROMINOS_HPP_ */

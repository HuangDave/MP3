/*
 * Tetrominos.cpp
 *
 *  Created on: May 14, 2018
 *      Author: huang
 */

#include <MP3/Games/Tetris/Tetrominos.hpp>

const uint8_t i_shape[4][4] = {
    {
        0b00000000,
        0b11110000,
        0b00000000,
        0b00000000,
    },
    {
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
    },
    {
        0b00000000,
        0b00000000,
        0b11110000,
        0b00000000,
    },
    {
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
    }
};

Tetrominos::Tetrominos() {
    // TODO Auto-generated constructor stub

}

Tetrominos::~Tetrominos() {
    // TODO Auto-generated destructor stub
}

uint8_t* Tetrominos::getBitmap(Shape shape, Orientation orientation) const {

}

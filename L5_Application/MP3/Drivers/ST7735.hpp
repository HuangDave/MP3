/*
 * ST7735.hpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#ifndef ST7735_HPP_
#define ST7735_HPP_

#include "SPI.hpp"

#include <MP3/gfx.h>

#define LCDDisplay ST7735::sharedInstance()

#define ST7735_TFT_WIDTH  128
#define ST7735_TFT_HEIGHT 160

/**
 * ST7735 operates at ~12MHz.
 *
 */
class ST7735: public SPI {

protected:

    static ST7735 *instance;

    LabGPIO *mpDC;
    LabGPIO *mpRESET;

    ST7735();

    // Toggle D/C low to interpret transmission byte as command byte.
    void selectDC();
    // Toggle D/C high to interpret transmission byte as data byte.
    void deselectDC();

    uint8_t write(uint8_t data);
    uint16_t writeWord(uint16_t data);
    void writeCommand(uint8_t cmd);

public:

    static ST7735& sharedInstance();

    virtual ~ST7735();

    void toggleRESET();
    void toggleSleep(bool on);
    void toggleDisplay(bool on);

    void writeColor(Color color, uint32_t repeat = 1);

    void setAddrWindow(Frame frame);
    void fillRect(Frame frame, Color c);
    void drawFont(Point2D p, const uint8_t *font, Color color, Color backgroundColor);
    void drawBitmap(Frame frame, const uint8_t *bitmap, Color color, Color backgroundColor);
    void drawBitmap(Frame frame, const uint16_t *bitmap, Color color, Color backgroundColor);
};

#endif /* ST7735_HPP_ */
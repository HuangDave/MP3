/*
 * ST7735.hpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#ifndef ST7735_HPP_
#define ST7735_HPP_

#include "../L4/LabSPI.hpp"

#include "gfx.h"

#define LCDDisplay ST7735::sharedInstance()

#define ST7735_TFT_WIDTH  128
#define ST7735_TFT_HEIGHT 160

// swap width and height of LCD display for drawing due to X-Y exchange orientation
#define SCREEN_WIDTH      160
#define SCREEN_HEIGHT     128

/**
 * ST7735 operates at ~12MHz.
 *
 */
class ST7735: public LabSPI {

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

    //void setAddrWindow(Point2D p0, Point2D p1);
    void writeColor(Color color, uint32_t repeat = 1);
    //void drawPixel(Point2D p, Color c);
    //void drawLine(Point2D p0, Point2D p1, Color c);
    //void fillRect(Point2D p0, Point2D p1, Color c);

    void setAddrWindow(Frame frame);
    void fillRect(Frame frame, Color c);

    void drawFont(Point2D p, const uint8_t *bitmap, Color color, Color backgroundColor);
};

#endif /* ST7735_HPP_ */

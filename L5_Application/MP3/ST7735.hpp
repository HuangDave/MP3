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
    void writeColor(Color color, uint32_t repeat = 1);

    //void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

public:

    /// ST7735 display identification information.
    typedef struct {
        uint8_t manufacturerID;
        uint8_t versionID;
        uint8_t driverID;
    } DeviceInfo;

    static ST7735& sharedInstance();

    virtual ~ST7735();

    /// Gets the 24-bit display identification information
    //DeviceInfo getDeviceInfo();

    void lcd_delay(uint32_t ms);

    void toggleRESET();
    void toggleSleep(bool on);
    void toggleDisplay(bool on);

    void setAddrWindow(Point2D p0, Point2D p1);
    void drawPixel(Point2D p, Color c);
    void drawLine(Point2D p0, Point2D p1, Color c);
    void fillRect(Point2D p0, Point2D p1, Color c);
};

#endif /* ST7735_HPP_ */

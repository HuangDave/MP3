/*
 * ST7735.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#include <MP3/Drivers/ST7735.hpp>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "utilities.h"

#define ST7735_SWRESET (0x01)   // Software RESET
#define ST7735_RDDID   (0x04)   // Read 24-bit device identification

#define ST7735_CASET   (0x2A)   // Column Address Set
#define ST7735_RASET   (0x2B)   // Row Address Set

#define ST7735_RAMWR   (0x2C)   // write to memeory
#define ST7735_RAMRD   (0x2E)   // read from memeory
#define ST7735_SLPIN   (0x10)   // Sleep In
#define ST7735_SLPOUT  (0x11)   // Sleep Out
#define ST7735_DISPOFF (0x28)   // Display ON
#define ST7735_DISPON  (0x29)   // Display OFF
#define ST7735_MADCTL  (0x36)   // Write Direction

ST7735* ST7735::instance = NULL;

ST7735& ST7735::sharedInstance() {
    if (instance == NULL) instance = new ST7735();
    return *instance;
}

ST7735::ST7735() {
    init(SSP1, DATASIZE_8_BIT, FRAMEMODE_SPI, PCLK_DIV_1);

    // configure pclk to be ~12 MHz = (clk / pclk_div) / CPSDVSR * [SCR+1],
    // pclk_div = 1, CPSDVSR = 4, SCR = 0
    SSPn->CR0 &= ~(1 << 8);
    SSPn->CPSR = 4;                                 // minimum prescaler of 2

    SSPn->CR1 &= ~(1 << 2);                         // clear MS to enable SSP as master

    mpCS    = configureGPIO(2, 7, true, true);      // configure P2.9 for CS (chip select)
    mpDC    = configureGPIO(2, 8, true, true);      // configure P0.1 for A0 (D/C data transfer select)
    mpRESET = configureGPIO(2, 9, true, true);      // configure P0.0 for RESET

    toggleRESET();
}

ST7735::~ST7735() {
    delete mpDC;
    delete mpRESET;
}

void ST7735::toggleRESET() {
    selectCS();
    {
        mpRESET->setLow();
        delay_ms(10);
        mpRESET->setHigh();
        delay_ms(10);
    }
    deselectCS();

    writeCommand(ST7735_SLPOUT);
    delay_ms(10);
    writeCommand(ST7735_DISPON);
    delay_ms(10);

    // set initial display to white
    fillRect(Frame { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, WHITE_COLOR);

    // set orientation X-Y exchange
    writeCommand(ST7735_MADCTL);
    write(0x84);
}

void ST7735::toggleSleep(bool on) {
    selectDC();
    {
        //selectCS();
        SSPn->CPSR = 4;                                 // configure 12MHz pclk
        transfer(on ? ST7735_DISPON : ST7735_DISPOFF);
        //deselectCS();
        delay_ms(10);
    }
    deselectDC();
}

void ST7735::toggleDisplay(bool on) {
    selectDC();
    {
        //selectCS();
        SSPn->CPSR = 4;                                 // configure 12MHz pclk
        transfer(on ? ST7735_DISPON : ST7735_DISPOFF);
        //deselectCS();
        delay_ms(10);
    }
    deselectDC();
}

inline void ST7735::selectDC() {
    if (xSemaphoreTake(spiMutex[mPeripheral], portMAX_DELAY)) {
        mpDC->setLow();
        mpCS->setLow();
    }
}

inline void ST7735::deselectDC() {
    mpCS->setHigh();
    mpDC->setHigh();
    xSemaphoreGive(spiMutex[mPeripheral]);
}

inline uint8_t ST7735::write(uint8_t data) {
    uint8_t byte = 0x00;
    selectCS();
    {
        SSPn->CPSR = 2;                                 // configure 12MHz pclk
        byte = transfer(data);
    }
    deselectCS();
    return byte;
}

inline uint16_t ST7735::writeWord(uint16_t data) {
    uint16_t word = 0x0000;
    word = (write(data >> 8) << 8) & 0xFF00;
    word = write(data & 0xFF) & 0xFF;
    return word;
}

inline void ST7735::writeCommand(uint8_t cmd) {
    selectDC();                                     // set D/C low to enable data command transmission
    {
        SSPn->CPSR = 2;                                 // configure 12MHz pclk
        transfer(cmd);
    }
    deselectDC();
}

inline void ST7735::writeColor(Color color, uint32_t repeat) {
    while (repeat > 0) {
        write(color.r);
        write(color.g);
        write(color.b);
        repeat--;
    }
}

inline void ST7735::setAddrWindow(Frame frame) {
    // Using orientation: X-Y exchange
    // swap x with y and width with height when setting address windrow
    writeCommand(ST7735_CASET);                     // write x-component of address window
    writeWord(frame.y);
    writeWord(frame.y + frame.height - 1);
    writeCommand(ST7735_RASET);                     // write y-component of address window
    writeWord(frame.x);
    writeWord(frame.x + frame.width - 1);
    writeCommand(ST7735_RAMWR);                     // write to memory
}

void ST7735::fillRect(Frame frame, Color c) {
    setAddrWindow(frame);
    writeColor(c, frame.width * frame.height);
}

void ST7735::drawFont(Point2D p, const uint8_t *font, Color color, Color backgroundColor) {
    const uint8_t kFontWidth  = 5;
    const uint8_t kFontHeight = 8;
    drawBitmap(Frame { p.x, p.y, kFontWidth, kFontHeight }, font, color, backgroundColor);
}

void ST7735::drawBitmap(Frame frame, const uint8_t *bitmap, Color color, Color backgroundColor) {
    setAddrWindow(frame);
    for (uint8_t y = 0; y < frame.width; y++) {
        for (uint8_t x = 0; x < frame.height; x++) {
            writeColor( !!(bitmap[y] & (1 << x)) ? color : backgroundColor );
        }
    }
}

void ST7735::drawBitmap(Frame frame, const uint16_t *bitmap, Color color, Color backgroundColor) {
    setAddrWindow(frame);
    for (uint8_t y = 0; y < frame.width; y++) {
        for (uint8_t x = 0; x < frame.height; x++) {
            writeColor( !!(bitmap[y] & (1 << x)) ? color : backgroundColor );
        }
    }
}

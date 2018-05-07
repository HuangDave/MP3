/*
 * ST7735.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#include <MP3/ST7735.hpp>
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
#define swap(x, y) { x = x + y; y = x - y; x = x - y ; }

ST7735* ST7735::instance = NULL;

ST7735& ST7735::sharedInstance() {
    if (instance == NULL) instance = new ST7735();
    return *instance;
}

ST7735::ST7735() {
    init(SSP0, DATASIZE_8_BIT, FRAMEMODE_SPI, PCLK_DIV_1);

    // configure pclk to be ~12 MHz = (clk / pclk_div) / CPSDVSR * [SCR+1],
    // pclk_div = 1, CPSDVSR = 4, SCR = 0
    SSPn->CR0 &= ~(1 << 8);
    SSPn->CPSR = 4;                                 // minimum prescaler of 2

    SSPn->CR1 &= ~(1 << 2);                         // clear MS to enable SSP as master

    mpCS    = configureGPIO(2, 7, true, true);      // configure P2.9 for CS (chip select)
    mpDC    = configureGPIO(0, 1, true, true);      // configure P0.1 for A0 (D/C data transfer select)
    mpRESET = configureGPIO(0, 0, true, true);      // configure P0.0 for RESET

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
    //fillRect(Point2D { 0,0 }, Point2D { 128, 160 }, WHITE_COLOR);

    writeCommand(ST7735_MADCTL);
    write(0x84); // set orientation X-Y exchange
}

void ST7735::toggleSleep(bool on) {
    selectDC();
    {
        selectCS();
        transfer(on ? ST7735_DISPON : ST7735_DISPOFF);
        deselectCS();
        delay_ms(500);
    }
    deselectDC();
}

void ST7735::toggleDisplay(bool on) {
    selectDC();
    {
        selectCS();
        transfer(on ? ST7735_DISPON : ST7735_DISPOFF);
        deselectCS();
        delay_ms(500);
    }
    deselectDC();
}

void ST7735::selectDC() {
    mpDC->setLow();
}

void ST7735::deselectDC() {
    mpDC->setHigh();
}

uint8_t ST7735::write(uint8_t data) {
    uint8_t byte = 0x00;
    selectCS();
    {
        byte = transfer(data);
    }
    deselectCS();
    return byte;
}

uint16_t ST7735::writeWord(uint16_t data) {
    uint16_t word = 0x0000;
    word = (write(data >> 8) << 8) & 0xFF00;
    word = write(data & 0xFF) & 0xFF;
    return word;
}

void ST7735::writeCommand(uint8_t cmd) {
    selectDC();                                     // set D/C low to enable data command transmission
    {
        write(cmd);
    }
    deselectDC();
}

void ST7735::writeColor(Color color, uint32_t repeat) {
    while (repeat > 0) {
        write(color.r);
        write(color.g);
        write(color.b);
        repeat--;
    }
}

void ST7735::setAddrWindow(Point2D p0, Point2D p1) {
    writeCommand(ST7735_CASET);                     // write x-component of address window
    writeWord(p0.x);
    writeWord(p1.x);
    writeCommand(ST7735_RASET);                     // write y-component of address window
    writeWord(p0.y);
    writeWord(p1.y);
    writeCommand(ST7735_RAMWR);                     // write to memory
}

void ST7735::setAddrWindow(Frame frame) {
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

void ST7735::drawPixel(Point2D p, Color c) {

    if ((p.x < 0) || (p.x >= ST7735_TFT_WIDTH) ||   // ensure point is within the boundary of the screen
        (p.y < 0) || (p.y >= ST7735_TFT_HEIGHT))
        return;

    Point2D p1;
    p1.x = p.x + 1; p1.y = p.y + 1;

    setAddrWindow(p, p1);
    writeColor(c);
}

void ST7735::drawLine(Point2D p0, Point2D p1, Color c) {
    int16_t slope = abs(p1.y - p0.y) > abs(p1.x - p0.x);

	if (slope) {
		swap(p0.x, p0.y);
		swap(p1.x, p1.y);
	}

	if (p0.x > p1.x) {
		swap(p0.x, p1.x);
		swap(p0.y, p1.y);
	}

	int16_t dx, dy;
	dx = p1.x - p0.x;
	dy = abs(p1.y - p0.y);

	int16_t err = dx / 2;
	int16_t ystep;

	if (p0.y < p1.y) ystep = 1;
	else 	         ystep = -1;

	for (; p0.x <= p1.x; p0.x++) {
		if (slope) drawPixel(Point2D { p0.y, p0.x }, c);
		else       drawPixel(Point2D { p0.x, p0.y }, c);

		err -= dy;

		if (err < 0) {
			p0.y += ystep;
			err += dx;
		}
	}
}

void ST7735::fillRect(Point2D p0, Point2D p1, Color c) {
	int16_t width, height;
	width = p1.x - p0.x + 1;
	height = p1.y - p0.y + 1;

	setAddrWindow(p0, p1);
	writeColor(c, width * height);
}

void ST7735::fillRect(Frame frame, Color c) {
    setAddrWindow(frame);
    writeColor(c, frame.width * frame.height);
}

void ST7735::drawFont(Point2D p, const uint8_t *bitmap, Color color, Color backgroundColor) {
    setAddrWindow( Frame { p.x, p.y, 5, 8 } );
    for (uint8_t y = 0; y < 5; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            writeColor( !!(bitmap[y] & (1 << x)) ? color : backgroundColor );
        }
    }
}

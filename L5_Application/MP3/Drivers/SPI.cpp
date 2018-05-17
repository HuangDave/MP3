/*
 * SPI.cpp
 *
 *  Created on: May 8, 2018
 *      Author: huang
 */

#include <MP3/Drivers/SPI.hpp>
#include <L1/LabGPIO.hpp>

volatile LPC_SSP_TypeDef* SPI::SSP[] = { LPC_SSP0, LPC_SSP1 };

SemaphoreHandle_t SPI::spiMutex[] = { xSemaphoreCreateMutex(), xSemaphoreCreateMutex() };

SPI::SPI() : SSPn(NULL), mpCS(NULL), mPeripheral(SSP0) { }

SPI::~SPI() {
    delete mpCS; mpCS = NULL;
}

bool SPI::init(SSP_Peripheral peripheral, DataSize dataSize, FrameMode mode, PCLK_DIV clkdiv) {
    switch (peripheral) {                                                       // enable PCLK for SPI
        case SSP0: enableSSP0(clkdiv); break;
        case SSP1: enableSSP1(clkdiv); break;
    }

    mPeripheral = peripheral;
    SSPn = SSP[mPeripheral];

    SSPn->CR0 = (dataSize << 0);                                                // 4-bit data size select (DSS)
    SSPn->CR1 |= (1 << 1);                                                      // enable SSE
    SSPn->CR1 &= ~(1 << 2);                                                     // clear MS to enable SSP as master

    return true;
}

void SPI::enableSSP0(PCLK_DIV clkdiv) {
    LPC_SC->PCONP |= (1 << 21);

    LPC_SC->PCLKSEL1 &= ~(3 << 10);                                             // enable pclk for PCLK_SSP0
    LPC_SC->PCLKSEL1 |= (clkdiv << 10);

    LPC_PINCON->PINSEL0 &= ~(3 << 30);                                          // enable SCK0
    LPC_PINCON->PINSEL0 |=  (2 << 30);
    LPC_PINCON->PINSEL1 &= ~(3 << 2) | ~(3 << 4);                               // enalbe MISO0, MOSI0
    LPC_PINCON->PINSEL1 |=  (2 << 2) |  (2 << 4);
}

void SPI::enableSSP1(PCLK_DIV clkdiv) {
    LPC_SC->PCONP |= (1 << 10);

    LPC_SC->PCLKSEL0 &= ~(3 << 20);                                             // enable pclk for PCLK_SSP1
    LPC_SC->PCLKSEL0 |= (clkdiv << 20);

    LPC_PINCON->PINSEL0 &= ~(3 << 14) | ~(3 << 16) | ~(3 << 18);                // enable SCK1, MISO1, MOSI1
    LPC_PINCON->PINSEL0 |=  (2 << 16) |  (2 << 18);
}

void SPI::selectCS() {
    if (xSemaphoreTake(spiMutex[mPeripheral], portMAX_DELAY))
        mpCS->setLow();
}

void SPI::deselectCS() {
    mpCS->setHigh();
    xSemaphoreGive(spiMutex[mPeripheral]);
}

uint8_t SPI::transfer(uint8_t data) {
    SSPn->DR = data;                                                            // SSPn->DR 16-bit data RW
    while (SSPn->SR & (1 << 4));                                                // wait on SSP status register
    return SSPn->DR;
}

uint16_t SPI::transferWord(uint16_t data) {
    uint16_t received = 0;
    received  = transfer(data >> 8) << 8;                                       // send MSB half of data
    received |= transfer((data & 0xFF) & 0xFF);                                 // send LSB half of data
    return received;
}

uint8_t* SPI::transfer(uint8_t *data, uint32_t len) {
    uint8_t in[len];
    for (uint32_t i = 0; i < len; i++)
        in[i] = transfer(data[i]);
    return in;
}

uint16_t* SPI::transferWord(uint16_t *data, uint32_t len) {
    uint16_t in[len];
    for (uint32_t i = 0; i < len; i++)
        in[i] = transferWord(data[i]);
    return in;
}

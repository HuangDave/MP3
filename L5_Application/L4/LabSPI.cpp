/*
 * LabSPI.cpp
 *
 *  Created on: Mar 1, 2018
 *      Author: huang
 */

#include <L4/LabSPI.hpp>
#include <stddef.h>
#include <stdio.h>

volatile LPC_SSP_TypeDef* LabSPI::SSP[] = { LPC_SSP0, LPC_SSP1 };

SemaphoreHandle_t LabSPI::spiMutex[] = { xSemaphoreCreateMutex(), xSemaphoreCreateMutex() };

LabSPI::LabSPI() : SSPn(NULL), mpCS(NULL), mPeripheral(SSP0) { }

LabSPI::~LabSPI() { }

bool LabSPI::init(SSP_Peripheral peripheral, DataSize dataSize, FrameMode mode, PCLK_Rate rate) {
    switch (peripheral) {                                                       // enable PCLK for SPI
        case SSP0: enableSSP0(rate); break;
        case SSP1: enableSSP1(rate); break;
    }

    mPeripheral = peripheral;
    SSPn = SSP[mPeripheral];

    SSPn->CR0 = (dataSize << 0);                                               // 4-bit data size select (DSS)
    // SSPn->CR0 |= (0xAA << 8);                                     // 8-bit serial clock rate (SCR), f_spi = PCLK / (CPSDVSR * [SCR + 1])

    SSPn->CR1 |= (1 << 1);                                                      // enable SSE
    SSPn->CR1 &= ~(1 << 2);                                                   // clear MS to enable SSP as master

    return true;
}

void LabSPI::enableSSP0(PCLK_Rate rate) {
    LPC_SC->PCONP |= (1 << 21);

    LPC_SC->PCLKSEL1 &= ~(3 << 10);                                             // enable pclk for PCLK_SSP0
    LPC_SC->PCLKSEL1 |= (rate << 10);

    LPC_PINCON->PINSEL0 &= ~(3 << 30);                                          // enable SCK0
    LPC_PINCON->PINSEL0 |=  (2 << 30);
    LPC_PINCON->PINSEL1 &= ~(3 << 2) | ~(3 << 4);                               // enalbe MISO0, MOSI0
    LPC_PINCON->PINSEL1 |=  (2 << 2) |  (2 << 4);
}

void LabSPI::enableSSP1(PCLK_Rate rate) {
    LPC_SC->PCONP |= (1 << 10);

    LPC_SC->PCLKSEL0 &= ~(3 << 20);                                             // enable pclk for PCLK_SSP1
    LPC_SC->PCLKSEL0 |= (rate << 20);

    LPC_PINCON->PINSEL0 &= ~(3 << 14) | ~(3 << 16) | ~(3 << 18);                // enable SCK1, MISO1, MOSI1
    LPC_PINCON->PINSEL0 |=  (2 << 16) |  (2 << 18);
}

LabGPIO* LabSPI::configureGPIO(uint8_t port, uint32_t pin, bool output, bool high) {
    LabGPIO *gpio = new LabGPIO(port, pin);
    gpio->setDirection(output);
    if (output) gpio->set(high);
    return gpio;
}

void LabSPI::selectCS() {
    if (xSemaphoreTake(spiMutex[mPeripheral], portMAX_DELAY))
        mpCS->setLow();
}

void LabSPI::deselectCS() {
    mpCS->setHigh();
    xSemaphoreGive(spiMutex[mPeripheral]);
}

uint8_t LabSPI::transfer(uint8_t data) {
    SSPn->DR = data;                                                            // SSPn->DR 16-bit data RW
    while (SSPn->SR & (1 << 4));                                                // wait on SSP status register
    return SSPn->DR;
}

uint16_t LabSPI::transferWord(uint16_t data) {
    uint16_t received = 0;
    received  = transfer(data >> 8) << 8;                                       // send MSB half of data
    received |= transfer((data & 0xFF) & 0xFF);                                 // send LSB half of data
    return received;
}

uint8_t* LabSPI::transfer(uint8_t *data, uint32_t len) {
    uint8_t in[len];
    for (uint32_t i = 0; i < len; i++)
        in[i] = transfer(data[i]);
    return in;
}

uint16_t* LabSPI::transferWord(uint16_t *data, uint32_t len) {
    uint16_t in[len];
    for (uint32_t i = 0; i < len; i++)
        in[i] = transferWord(data[i]);
    return in;
}

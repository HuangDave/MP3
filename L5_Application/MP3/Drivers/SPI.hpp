/*
 * SPI.hpp
 *
 *  Created on: May 8, 2018
 *      Author: huang
 */

#ifndef SPI_HPP_
#define SPI_HPP_

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "semphr.h"

class LabGPIO;

class SPI {

public:

    typedef enum {
        SSP0 = 0,
        SSP1
    } SSP_Peripheral;

    typedef enum {
        PCLK_DIV_4 = 0b00,
        PCLK_DIV_1 = 0b01,
        PCLK_DIV_2 = 0b10,
        PCLK_DIV_8 = 0b11
    } PCLK_DIV;

    typedef enum {
        DATASIZE_4_BIT  = 0x3,
        DATASIZE_5_BIT  = 0x4,
        DATASIZE_6_BIT  = 0x5,
        DATASIZE_7_BIT  = 0x6,
        DATASIZE_8_BIT  = 0x7,
        DATASIZE_9_BIT  = 0x8,
        DATASIZE_10_BIT = 0x9,
        DATASIZE_11_BIT = 0xA,
        DATASIZE_12_BIT = 0xB,
        DATASIZE_13_BIT = 0xC,
        DATASIZE_14_BIT = 0xD,
        DATASIZE_15_BIT = 0xE,
        DATASIZE_16_BIT = 0xF
    } DataSize;

    typedef enum {
        FRAMEMODE_SPI       = 0x0,
        FRAMEMODE_TI        = 0x1,
        FRAMEMODE_MICROWAVE = 0x2
    } FrameMode;

    typedef union {
        uint32_t bytes;
        struct {
            uint32_t TFE : 1;   // transmit FIFO empty
            uint32_t TNF : 1;   // transmit FIFI not full
            uint32_t RNE : 1;   // recieve FIFI empty
            uint32_t RFF : 1;   // recieve FIFI not full
            uint32_t BUSY: 1;   // SSP is busy
            uint32_t reserved: 27;
        } __attribute__((packed));
    } SSP_SR;

    /// Array containing a CS mutex for SSP0 and SSP1. Locks the SPI bus when a slave is selected.
    static SemaphoreHandle_t spiMutex[2];

    /**
     * 1) Powers on SPPn peripheral
     * 2) Set peripheral clock
     * 3) Sets pins for specified peripheral to MOSI, MISO, and SCK
     *
     * @param peripheral which peripheral SSP0 or SSP1 you want to select.
     * @param dataSize   transfer size data width; To optimize the code, look for a pattern in the datasheet
     * @param format     is the code format for which synchronous serial protocol you want to use.
     * @param clkdiv     is the how much to divide the clock for SSP; take care of error cases such as the value of 0, 1, and odd numbers
     *
     * @return           Returns true if initialization was successful
     */
    bool init(SSP_Peripheral peripheral, DataSize dataSize, FrameMode mode, PCLK_DIV clkdiv);

    /// Locks the SPI bus and sets CS to low if successfull.
    void selectCS();
    /// Unlocks the SPI buss and sets CS to high.
    void deselectCS();

    /**
     * Transfers a byte via SSP to an external device using the SSP data register.
     * This region must be protected by a mutex static to this class.
     *
     * @param  data     8-bit data byte to transfer
     * @return          Returns the received byte from external device via SSP data register.
     */
    uint8_t   transfer(uint8_t data);
    uint8_t*  transfer(uint8_t *data, uint32_t len);
    uint16_t  transferWord(uint16_t data);
    uint16_t* transferWord(uint16_t *data, uint32_t len);

    // Destructor
    ~SPI();

protected:

    volatile LPC_SSP_TypeDef *SSPn;

    /// Used in master mode for slave sel.
    LabGPIO *mpCS;

    SSP_Peripheral mPeripheral;

    // Protected constructor.
    SPI();

private:

    static volatile LPC_SSP_TypeDef *SSP[];

    void enableSSP0(PCLK_DIV clkdiv);
    void enableSSP1(PCLK_DIV clkdiv);

};

#endif /* SPI_HPP_ */

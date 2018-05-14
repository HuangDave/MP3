/*
 * VS1053B.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#include <MP3/Drivers/VS1053B.hpp>
#include <stddef.h>
#include "io.hpp"
#include <stdio.h>
#include "utilities.h"

#define SCI_WRITE       (0x02)
#define SCI_READ        (0x03)

#define RESYNC_BYTES    (0x1E29)
#define END_FILL_BYTES  (0x1E06)

#define SCI_SOFT_RESET   ( SM_SDINEW | SM_RESET )
#define SCI_MODE_DEFAULT ( SM_LINE1 | SM_SDINEW )
#define SCI_MODE_STREAM  ( SM_LINE1 | SM_SDINEW | SM_STREAM )
#define SCI_MODE_CANCEL  ( SM_LINE1 | SM_SDINEW | SM_CANCEL )

const uint16_t VS1053B::sampleRateLUT[4][4] = {
    { 11025, 11025, 22050, 44100 },
    { 12000, 12000, 24000, 48000 },
    {  8000,  8000, 16000, 32000 },
    {     0,     0,     0,     0 }
};

VS1053B* VS1053B::instance = NULL;

VS1053B& VS1053B::sharedInstance() {
    if (instance == NULL) instance = new VS1053B();
    return *instance;
}

VS1053B::VS1053B() {
    init(SSP0, DATASIZE_8_BIT, FRAMEMODE_SPI, PCLK_DIV_1);

    SSPn->CPSR = 16; // set initial pclk to ~3MHz = 12MHz / 4 for SCI write at reset
    //SSPn->CR0 &= ~(0xFF << 8); // set SCR to 0

    mpDREQ  = configureGPIO(2, 4, false, false);           // Configure P2.4 as input for DREQ
    mpRESET = configureGPIO(2, 3, true,  true);            // configure P2.3 for RESET
    mpCS    = configureGPIO(2, 2, true,  true);            // configure P2.2 for CS
    mpSDCS  = configureGPIO(2, 1, true,  true);            // Configure P2.1 for SDCS
    mpXDCS  = configureGPIO(2, 0, true,  true);            // Configure P2.0 for XDCS

    reset();

    writeREG(SCI_MODE,   SCI_MODE_DEFAULT);
    writeREG(SCI_CLOCKF, CLOCKF_MULT_4x);                   // set multiplier to 4.0x
    setVolume(255);
}

VS1053B::~VS1053B() {
    delete mpDREQ;  mpDREQ  = NULL;
    delete mpRESET; mpRESET = NULL;
    delete mpSDCS;  mpSDCS  = NULL;
    delete mpXDCS;  mpXDCS  = NULL;
}

void VS1053B::reset() {
    mpRESET->setHigh();
    mpRESET->setLow();
    delay_ms(1);
    mpRESET->setHigh();

    while(!isReady());

    mState = STOPPED;
}

void VS1053B::softReset() {
    writeSCI(SCI_MODE, SCI_SOFT_RESET);
}

bool VS1053B::isReady() {
    return (*mpDREQ).getLevel();
}

VS1053B::DecoderState VS1053B::getState() {
    return mState;
}

uint16_t VS1053B::readSCI(uint8_t addr) {
    uint16_t data;
    while(!isReady());
    selectCS();
    {
        SSPn->CPSR = 16;                                        // SCK needs to match CLKI / 7 for SCI rw
        transfer(SCI_READ);
        transfer(addr);
        data = transferWord(0x00);
    }
    deselectCS();
    return data;
}

void VS1053B::writeSCI(uint8_t addr, uint16_t data) {
    writeSCI(addr, &data, 1);
}

void VS1053B::writeSCI(uint8_t addr, uint16_t *data, uint32_t len) {
    while(!isReady());
    selectCS();
    {
        SSPn->CPSR = 16;                                        // SCK needs to match CLKI / 7 for SCI rw
        transfer(SCI_WRITE);
        transfer(addr);
        for (uint32_t i = 0; i < len; i++) {
            transferWord(data[i]);
        }
    }
    deselectCS();
}

void VS1053B::writeSDI(uint8_t data) {
    writeSDI(&data, 1);
}

void VS1053B::writeSDI(uint8_t *data, uint32_t len) {
    while(!isReady());
    if (xSemaphoreTake(spiMutex[mPeripheral], portMAX_DELAY)) {
        mpXDCS->setLow();
        {
            SSPn->CPSR = 4;                                     // SCK needs to match CLKI / 4 for for SDI writes
            transfer(data, len);
        }
        mpXDCS->setHigh();
        xSemaphoreGive(spiMutex[mPeripheral]);
    }
}

uint16_t VS1053B::readREG(uint8_t addr) {
    return readSCI(addr);
}

void VS1053B::writeREG(uint8_t addr, uint16_t reg) {
    writeSCI(addr, reg);
}

void VS1053B::setVolume(uint8_t volume) {
    uint8_t vol = 0xFF - volume; // find difference since max volume for device is 0x00 and no volume is 0xFF
    // VS_VOL 16-bit reg controls the volume for both the left and right channels
    writeSCI(SCI_VOL, ((vol << 8) | vol));
}

void VS1053B::clearDecodeTime() {
    writeSCI(SCI_DECODE_TIME, 0x00);
    writeSCI(SCI_DECODE_TIME, 0x00);
}

VS1053B::HeaderData VS1053B::getHDAT() {
    HeaderData hdat;
    hdat.bytes = (readREG(SCI_HDAT1) << 16) | readREG(SCI_HDAT0);
    return hdat;
}

void VS1053B::enablePlayback() {
    writeREG(SCI_MODE, SCI_MODE_DEFAULT);
    writeREG(SCI_AUDATA, (AUDATA_44100 | AUDATA_STEREO));

    writeSCI(SCI_WRAMADDR, 0x1E29);                         // Automatic Resync selector
    writeSCI(SCI_WRAM, 0);

    clearDecodeTime();                                      // reset current decode time to 0:00

    mState = PLAYING;
}

void VS1053B::disablePlayback() {
    mState = CANCELLING;
    writeREG(SCI_MODE, SCI_MODE_CANCEL);

    // wait for DREQ to be high and SM_CANCEL to be cleared...
    while (!isReady() || (readREG(SCI_MODE) & SM_CANCEL));

    mState = PAUSED;
}

void VS1053B::buffer(uint8_t *songData, uint32_t len) {
    writeSDI(songData, len);
}

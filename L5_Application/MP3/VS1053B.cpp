/*
 * VS1053B.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#include <MP3/VS1053B.hpp>
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

typedef enum {
    SM_DIFF          = (1 << 0),  // Differential: 0 = normal in-phase audio, 1 = left channel inverted
    SM_LAYER12       = (1 << 1),  // Allow MPEG layers I & II
    SM_RESET         = (1 << 2),  // Software RESET: 0 = no reset, 1 = reset
    SM_CANCEL        = (1 << 3),  // Cancel decoding current file
    SM_EARSPEAKER_LO = (1 << 4),  // EarSpeaker low setting: 0 = off, 1 = active
    SM_TESTS         = (1 << 5),  // Allow SDI tests
    SM_STREAM        = (1 << 6),  // Stream mode
    SM_EARSPEAKER_HI = (1 << 7),  // EarSpeaker high setting: 0 = off, 1 = active
    SM_DACT          = (1 << 8),  // DCLK active edge: 0 = rising, 1 = falling
    SM_SDIORD        = (1 << 9),  // SDI bit order: 0 = MSb first, 1 = MSb last
    SM_SDISHARE      = (1 << 10), // Share SPI chip select
    SM_SDINEW        = (1 << 11), // VS1002 native SPI modes: 0 = VS1001 Compatibility Mode, 1 = VS10xx New Mode
    SM_ADPCM         = (1 << 12), // PCM/ADPCM recording active
    SM_LINE1         = (1 << 14), // MIC / LINE1 selector: 0 = MICP, 1 = LINE1
    SM_CLK_RANGE     = (1 << 15) // Input clock range: 0 = 12-13MHz, 1 = 24-26MHz
} SCI_MODE_OPTIONS;

typedef enum {
    SS_REFERENCE_SEL = (1 << 0),  // Reference voltage selection, ’0’ = 1.23 V, ’1’ = 1.65 V
    SS_AD_CLOCK      = (1 << 1),  // AD clock select, ’0’ = 6 MHz, ’1’ = 3 MHz
    SS_APDOWN1       = (1 << 2),  // Analog internal powerdown
    SS_APDOWN2       = (1 << 3),  // Analog driver powerdown
    SS_VER           = (1 << 4),  // Version
    SS_VCM_DISABLE   = (1 << 10), // GBUF overload detection ’1’ = disable
    SS_VCM_OVERLOAD  = (1 << 11), // GBUF overload indicator ’1’ = overload
    SS_SWING         = (1 << 12), // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
    SS_DO_NOT_JUMP   = (1 << 15), // Header in decode, do not fast forward/rewind
} SCI_STATUS_OPTIONS;

typedef enum {
    SC_MULT_1x   = 0x0000,
    SC_MULT_2x   = 0x2000,
    SC_MULT_2_5x = 0x4000,
    SC_MULT_3x   = 0x6000,
    SC_MULT_3_5x = 0x8000
} SCI_CLOCKF_OPTIONS;

typedef enum {
    AUDATA_STEREO = (1 << 0),
    AUDATA_44100  = 44100,
} SCI_AUDATA_OPTIONS;

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

    // set initial pclk to 3MHz = 12MHz / 4 for write at reset
    SSPn->CPSR = 16;                                // minimum prescaler of 2
    SSPn->CR0 |= (0x0 << 8);

    mpDREQ  = configureGPIO(1, 30, false, false);   // Configure P1.30 as input for DREQ
    mpRESET = configureGPIO(0,  1, true, true);     // configure P0.1  for RESET
    mpCS    = configureGPIO(0,  0, true, true);     // configure P0.0  for CS
    mpSDCS  = configureGPIO(1, 31, true, true);     // Configure P1.31 for SDCS
    mpXDCS  = configureGPIO(2,  7, true, true);     // Configure P1.29 for XDCS

    reset();

    writeREG(SCI_MODE, SCI_MODE_DEFAULT);
    writeREG(SCI_CLOCKF, 0x6000);                   // set multiplier to 3.0x
    setVolume(150);

    while(!isReady()) delay_ms(3);
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
    delay_ms(2);
    mpRESET->setHigh();

    while(!isReady()) delay_ms(0.003);

    mIsPlaying = false;
}

void VS1053B::softReset() {
    writeSCI(SCI_MODE, SM_RESET);
}

bool VS1053B::isReady() { return mpDREQ->getLevel(); }

uint16_t VS1053B::readSCI(uint8_t addr) {
    uint16_t data;
    selectCS();
    {
        while(!isReady()) delay_ms(0.03);
        SSPn->CPSR = 16; // SCK needs to match CLKI / 7 for SCI rw
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
    selectCS();
    {
        while (!isReady()) delay_ms(0.03);
        SSPn->CPSR = 16; // SCK needs to match CLKI / 7 for SCI rw
        transfer(SCI_WRITE);
        transfer(addr);
        for (uint32_t i = 0; i < len; i++) {
            transferWord(data[i]);
            while (!isReady()) delay_ms(0.03);
        }
    }
    deselectCS();
    while(!isReady());
}

void VS1053B::writeSDI(uint8_t data) {
    writeSDI(&data, 1);
}

void VS1053B::writeSDI(uint8_t *data, uint32_t len) {
    if (xSemaphoreTake(spiMutex[mPeripheral], portMAX_DELAY)) {
        SSPn->CPSR = 4; // SCK needs to match CLKI / 4 for for SDI writes
        mpXDCS->setLow();
        {
            transfer(data, len);
        }
        mpXDCS->setHigh();
        xSemaphoreGive(spiMutex[mPeripheral]);
    }
}

void VS1053B::clearDecodeTime() {
    writeSCI(SCI_DECODE_TIME, 0x00);
    writeSCI(SCI_DECODE_TIME, 0x00);
}

uint16_t VS1053B::readREG(uint8_t addr) {
    return readSCI(addr);
}

void VS1053B::writeREG(uint8_t addr, uint16_t reg) {
    writeSCI(addr, reg);
}

void VS1053B::setVolume(uint8_t volume) {
    writeSCI(SCI_VOL, ((volume << 8) | volume));  // VS_VOL 16-bit reg controls the volume for both the left and right channels
}

VS1053B::HeaderData VS1053B::getHDAT() {
    HeaderData hdat;
    hdat.bytes = (readREG(SCI_HDAT1) << 16) | readREG(SCI_HDAT0);
    return hdat;
}

uint16_t VS1053B::getByteRate() {
    writeSCI(SCI_WRAMADDR, 0x1E29); // resync
    return readSCI(SCI_WRAM);
}

void VS1053B::sendEndFillBytes() {
    writeSCI(SCI_WRAMADDR, 0x1E06); // resync
    uint8_t byte = readSCI(SCI_WRAM);
    printf("endFillByte: %x\n", byte);
    for (uint32_t i = 0; i < 2052; i++)
        transfer(byte);
}

void VS1053B::playSong() {
    writeREG(SCI_MODE, SCI_MODE_STREAM);
    writeSCI(SCI_WRAMADDR, 0x1E29); // resync
    writeSCI(SCI_WRAM, 0);

    clearDecodeTime();
}

void VS1053B::buffer(uint8_t *songData, uint32_t len) {
    while (!isReady()) delay_ms(3);
    writeSDI(songData, len);
}

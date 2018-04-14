/*
 * VS1053B.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: huang
 */

#include <MP3/VS1053B.hpp>
#include <stddef.h>
#include "io.hpp"

#define SCI_WRITE        (0x02)
#define SCI_READ         (0x03)

// Device SCI Register Addresses
#define VS_MODE          (0x0)
#define VS_STATUS        (0x1)
#define VS_WRAM          (0x6)
#define VS_WRAMADDR      (0x7)
#define VS_VOL           (0xB)

#define RESYNC_BYTES     (0x1E29)
#define END_FILL_BYTES   (0x1E06)

#define VS_SOFT_RESET   ( SM_SDINEW | SM_RESET )
#define VS_STREAM       ( SM_LINE1 | SM_SDINEW | SM_STREAM )
#define VS_STOP         (  SM_LINE1 | SM_SDINEW | SM_CANCEL )

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
    SM_LINE1         = (1 << 15), // MIC / LINE1 selector: 0 = MICP, 1 = LINE1
    SM_CLK_RANGE     = (1 << 16) // Input clock range: 0 = 12-13MHz, 1 = 24-26MHz
} VS_MODE_OPTIONS;

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
} VS_STATUS_OPTIONS;

/*
// 9.6.3 SCI_BASS (RW) Built-in bass/treble control Register
typedef union {

} SCI_BASS;
*/

/*
// SCI_AUDDATA (RW)
typedef union {
    uint16_t bytes;
    struct {
        uint16_t channel    : 1;  // audio type: 0 = mono, 1 = stereo
        uint16_t sampleRate : 15; // Sample rate is the 15-bit value divided by 2
    } __attribute__((__packed__));
} SCI_AUDDATA;
*/

/*
// SCI_HDAT0 (R) Steam header data 0 register
typedef union {

} SCI_HDAT0;

// SCI_HDAT1 (R) Stream header data 1 register
typedef union {

} SCI_HDAT1;
*/

VS1053B* VS1053B::instance = NULL;

VS1053B& VS1053B::sharedInstance() {
    if (instance == NULL) instance = new VS1053B();
    return *instance;
}

/**
 * VS1053B operates at 12.288 MHz or 24-26 MHz when SM_CLK_RANGE in SCI_MODE is set to 1.
 */
VS1053B::VS1053B() {
    init(SSP1, DATASIZE_8_BIT, FRAMEMODE_SPI, PCLK_DIV_2);

    SSPn->CR0 &= ~(1 << 8);
    SSPn->CPSR = 2;                                 // minimum prescaler of 2
    SSPn->CR1 &= ~(1 << 2);                         // clear MS to enable SSP as master

    mpRESET = configureGPIO(0, 26, true, true);     // configure P0.26 for RESET
    mpCS    = configureGPIO(1, 31, true, true);     // configure P1.31 for CS
    mpDREQ  = configureGPIO(1, 30, false, false);   // Configure P1.30 as input for DREQ
    mpDCS   = configureGPIO(1, 29, true, true);     // Configure P1.29 for DCS

    mIsPlaying = false;
}

VS1053B::~VS1053B() { }

bool VS1053B::isReady() { return mpDREQ->getLevel(); }

uint16_t VS1053B::sciRead(uint8_t addr) {
    uint16_t data;
    selectCS();
    {
        transfer(SCI_READ);
        transfer(addr);
        data = transferWord(0x00);
    }
    deselectCS();
    return data;
}

void VS1053B::sciWrite(uint8_t addr, uint16_t data) {
    sciWrite(addr, &data, 1);
}

void VS1053B::sciWrite(uint8_t addr, uint16_t *data, uint32_t len) {
    selectCS();
    {
        transfer(SCI_WRITE);
        transfer(addr);
        transfer(data[0]);
        for (uint32_t i = 1; i < len; i++)
            transfer(data[i]);
    }
    deselectCS();
}

void VS1053B::enterSDIMODE() {
    if (xSemaphoreTake(spiMutex[mPeripheral], portMAX_DELAY))
        mpDCS->setLow();
}

void VS1053B::exitSDIMODE() {
    mpDCS->setHigh();
    xSemaphoreGive(spiMutex[mPeripheral]);
}

VS1053B::VS_SCI_STATUS VS1053B::status() {
    VS_SCI_STATUS status;
    status.bytes = sciRead(VS_STATUS);
    return status;
}

void VS1053B::setVolume(uint8_t volume) {
    sciWrite(VS_VOL, ((volume << 8) | volume));
}

void VS1053B::play() {
    sciWrite(VS_MODE, VS_STREAM);
}

void VS1053B::buffer(uint8_t *data, uint32_t len) {
    enterSDIMODE();
    {
        for (uint32_t i = 0; i < len; i++)
            transferWord(data[i]);
    }
    exitSDIMODE();
}

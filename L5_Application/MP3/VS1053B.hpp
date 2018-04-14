/*
* VS1053B.hpp
*
*  Created on: Apr 6, 2018
*      Author: huang
*/

#ifndef VS1053B_HPP_
#define VS1053B_HPP_

#include "../L4/LabSPI.hpp"

#define MP3 VS1053B::sharedInstance()

#define VS1053B_BUFFER_SIZE 32

class VS1053B: public LabSPI {

protected:

    /// 9.6.2 SCI_STATUS (RW) VS1053B status register
    typedef union {
        uint16_t bytes;
        struct {
            uint8_t SS_REFERENCE_SEL: 1; // Reference voltage selection, ’0’ = 1.23 V, ’1’ = 1.65 V
            uint8_t SS_AD_CLOCK     : 1; // AD clock select, ’0’ = 6 MHz, ’1’ = 3 MHz
            uint8_t SS_APDOWN1      : 1; // Analog internal powerdown
            uint8_t SS_APDOWN2      : 1; // Analog driver powerdown
            uint8_t SS_VER          : 4; // Version
            uint8_t                 : 2; // reserved
            uint8_t SS_VCM_DISABLE  : 1; // GBUF overload detection ’1’ = disable
            uint8_t SS_VCM_OVERLOAD : 1; // GBUF overload indicator ’1’ = overload
            uint8_t SS_SWING        : 3; // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
            uint8_t SS_DO_NOT_JUMP  : 1; // Header in decode, do not fast forward/rewind
        } __attribute__((__packed__));
    } VS_SCI_STATUS;

    /// 9.6.1 SCI_MODE (RW) Regsiter used to configure VS1053B operation.
    typedef union {
        uint16_t bytes;
        struct {
            uint8_t SM_DIFF         : 1; // Differential: 0 = normal in-phase audio, 1 = left channel inverted
            uint8_t SM_LAYER12      : 1; // Allow MPEG layers I & II
            uint8_t SM_RESET        : 1; // Software RESET: 0 = no reset, 1 = reset
            uint8_t SM_CANCEL       : 1; // Cancel decoding current file
            uint8_t SM_EARSPEAKER_LO: 1; // EarSpeaker low setting: 0 = off, 1 = active
            uint8_t SM_TESTS        : 1; // Allow SDI tests
            uint8_t SM_STREAM       : 1; // Stream mode
            uint8_t SM_EARSPEAKER_HI: 1; // EarSpeaker high setting: 0 = off, 1 = active
            uint8_t SM_DACT         : 1; // DCLK active edge: 0 = rising, 1 = falling
            uint8_t SM_SDIORD       : 1; // SDI bit order: 0 = MSb first, 1 = MSb last
            uint8_t SM_SDISHARE     : 1; // Share SPI chip select
            uint8_t SM_SDINEW       : 1; // VS1002 native SPI modes
            uint8_t SM_ADPCM        : 1; // PCM/ADPCM recording active
            uint8_t                 : 1; // reserved
            uint8_t SM_LINE1        : 1; // MIC / LINE1 selector: 0 = MICP, 1 = LINE1
            uint8_t SM_CLK_RANGE    : 1; // Input clock range: 0 = 12-13MHz, 1 = 24-26MHz
        } __attribute__((__packed__));
    } VS_SCI_MODE;

    static VS1053B *instance;

    // used for active low asynchronous reset
    LabGPIO *mpRESET;
    // Data Request bus from device. Signals execution of register update if driven low.
    LabGPIO *mpDREQ;
    // XDCS / BSYNC: Data chip select / byte sync
    LabGPIO *mpDCS;

    bool mIsPlaying;

    VS1053B();

    /**
    * Checks DREQ to determine if the device is performing a register update.
    * When a register update is executed, DREQ is driven low.
    *
    * @return Returns TRUE if DREQ is driven high.
    */
    bool isReady();

    uint16_t sciRead(uint8_t addr);
    void sciWrite(uint8_t addr, uint16_t data);
    void sciWrite(uint8_t addr, uint16_t *data, uint32_t len);

    // @return Returns the 16-bit SCI_STATUS register.
    VS_SCI_STATUS status();

    void enterSDIMODE();
    void exitSDIMODE();

public:

    static VS1053B& sharedInstance();

    virtual ~VS1053B();

    /**
    * Sets the volumes of the device.
    * The max volume is 0x00 while lowest volume is 0xFE.
    *
    * @param volume uint8_t ranging from 0x00 to 0xFE (0 - 254)
    */
    void setVolume(uint8_t volume);
    //void incrementVolume();
    //void decrementVolume();
    void play();
    void buffer(uint8_t *data, uint32_t len);
};

#endif /* VS1053B_HPP_ */

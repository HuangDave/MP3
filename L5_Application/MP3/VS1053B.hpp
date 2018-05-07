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

// Device SCI Register Addresses
#define SCI_MODE        (0x0)
#define SCI_STATUS      (0x1)
#define SCI_CLOCKF      (0x3)
#define SCI_DECODE_TIME (0x4)
#define SCI_AUDATA      (0x5)
#define SCI_WRAM        (0x6)
#define SCI_WRAMADDR    (0x7)
#define SCI_HDAT0       (0x8)
#define SCI_HDAT1       (0x9)
#define SCI_VOL         (0xB)

class VS1053B: public LabSPI {

protected:

    static VS1053B *instance;

    // used for active low asynchronous reset
    LabGPIO *mpRESET;
    // Data Request bus from device. Signals execution of register update if driven low.
    LabGPIO *mpDREQ;
    // XDCS / BSYNC: Data chip select / byte sync
    LabGPIO *mpXDCS;
    // Set to high to disable decoder SD read
    LabGPIO *mpSDCS;

    bool mIsPlaying;

    VS1053B();

    uint16_t readSCI(uint8_t addr);

    void writeSCI(uint8_t addr, uint16_t data);
    void writeSCI(uint8_t addr, uint16_t *data, uint32_t len);

    void writeSDI(uint8_t data);
    void writeSDI(uint8_t *data, uint32_t len);

    void writeREG(uint8_t addr, uint16_t reg);

public:

    typedef union {
        uint32_t bytes;

        struct {
            // HDAT0
            uint8_t  emphasis:       2;
            uint8_t  original:       1;
            uint8_t  copyright:      1;
            uint8_t  extension:      2;
            uint8_t  mode:           2; // 3: mono, 2: dual channel, 1: joint stereo, 0: stereo
            uint8_t  private_bit:    1; //
            uint8_t  pad_bit:        1; // 1: additional slot, 0: normal frame
            uint8_t  samplerate:     2; // 3: reserved, 2: 32/16/8 kHz, 1: 48/24/12 kHz, 0: 44/22/11 kHz
            uint8_t  bitrate:        4;

            // HDAT1
            uint8_t  protected_bit:  1;   // 1: no CRC, 0: CRC protected
            uint8_t  layer:          2;   // layer: 0x3 = 1, 0x2 = 2, 0x1 = 3, 0x0 = reserved
            uint8_t  id:             2;   // 0x3: ISO 11172-3 MPG 1.0
            uint16_t syncword:      11;   // stream valid should be 2047
        } __attribute__((packed));
    } HeaderData;

    static const uint16_t sampleRateLUT[4][4];

    static VS1053B& sharedInstance();

    virtual ~VS1053B();

    uint16_t readREG(uint8_t addr);

    void reset();
    void softReset();

    /**
    * Checks DREQ to determine if the device is performing a register update.
    * When a register update is executed, DREQ is driven low.
    *
    * @return Returns TRUE if DREQ is driven high.
    */
    bool isReady();

    HeaderData getHDAT();
    uint16_t getByteRate();
    
    /**
    * Sets the volumes of the device.
    * The max volume is 0x00 while lowest volume is 0xFE.
    *
    * @param volume uint8_t ranging from 0x00 to 0xFE (0 - 254)
    */
    void setVolume(uint8_t volume);
    //void incrementVolume();
    //void decrementVolume();

    void clearDecodeTime();

    void sendEndFillBytes();

    void playSong();
    void buffer(uint8_t *songData, uint32_t len);
};

#endif /* VS1053B_HPP_ */

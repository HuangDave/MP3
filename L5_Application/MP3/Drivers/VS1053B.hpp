/*
* VS1053B.hpp
*
*  Created on: Apr 6, 2018
*      Author: huang
*/

#ifndef VS1053B_HPP_
#define VS1053B_HPP_

#include "SPI.hpp"

#define MP3 VS1053B::sharedInstance()

#define VS1053B_BLOCK_SIZE  (512)
#define VS1053B_BUFFER_SIZE (32)

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

/**
 * VS1053B operates input clock freq. XTALI = 12.288 MHz or 24-26 MHz when SM_CLK_RANGE in SCI_MODE is set to 1.
 *
 * The SCI_CLOCKF register is used to increase the internal clock of the device.
 * CLKI = XTALI * multiplier
 *
 * When performing SCI reads, SPI clk should be ~(CLKI/7).
 * When performing SCI / SDI writes, SPI clk should be ~(CLKI/4).
 *
 * Therefore, on RESET, the intial SPI clock rate needs to be 12.288Mhz / 4 = ~3MHz.
 * Once the SCI_CLOCKF multiplier is set, the SPI clk can be changed correspondingly to faster speeds.
 */
class VS1053B: public SPI {

public:

    typedef enum {
        STOPPED    = (1 << 0),
        PLAYING    = (1 << 1),
        CANCELLING = (1 << 2)
    } DecoderState;

    // 32-bit Decoded Header Data
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

    /**
     * Performs a hardware reset.
     * RESET is toggled low for 2ms.
     * When RESET is driven high, wait for DREQ to be driven high before continueing.
     */
    void reset();

    /**
     * Performs a software reset by setting the SM_CANCEL bit of the SCI_MODE register.
     */
    void softReset();

    /**
    * Checks DREQ to determine if the device is performing a register update.
    * When a register update is executed, DREQ is driven low.
    *
    * @return Returns TRUE if DREQ is driven high.
    */
    bool isReady();

    DecoderState getState();

    /**
     * Read 32-bit decoded header data.
     * If no data is decoded, the data will be empty.
     * @return Returns a HeaderData.
     */
    HeaderData getHDAT();

    /**
    * Sets the volumes of the device.
    * The max volume is 0x00 while lowest volume is 0xFE.
    *
    * @param volume uint8_t ranging from 0x00 to 0xFE (0 - 254)
    */
    void setVolume(uint8_t volume);

    /**
     * Reset current decode time to 0:00.
     */
    void clearDecodeTime();

    /**
     * Enable device audio playback.
     */
    void enablePlayback();
    void disablePlayback();

    /**
     * Buffer song data to decoder.
     * @param songData Song data to buffer.
     * @param len      Length of buffer (ideally 32).
     */
    void buffer(uint8_t *songData, uint32_t len);


protected:

    static VS1053B *instance;

    /// used for active low asynchronous reset
    LabGPIO *mpRESET;
    /// Data Request status from device. Signals execution of register update if driven low.
    LabGPIO *mpDREQ;
    /// XDCS / BSYNC: Data chip select / byte sync
    LabGPIO *mpXDCS;
    /// Set to high to disable decoder SD read
    LabGPIO *mpSDCS;

    DecoderState mState;

    VS1053B();

    // SCI R/W

    /**
     * Read a data through SCI port.
     * @param  addr Address to read.
     * @return      Returns the 16-bit register data.
     */
    uint16_t readSCI(uint8_t addr);

    /**
     * Write 2 bytes of data to a SCI Regsiter.
     * @param addr SCI Register address.
     * @param data Data to write to SCI register.
     */
    void writeSCI(uint8_t addr, uint16_t data);

    /**
     * Write n-bytes of data to a SCI Regsiter.
     * @param addr SCI Register address.
     * @param data Pointer array.
     * @param len  Length of array.
     */
    void writeSCI(uint8_t addr, uint16_t *data, uint32_t len);

    // SDI Write

    /**
     * Send a byte of data using the SDI port for decoding.
     * @param data Byte of song data to send.
     */
    void writeSDI(uint8_t data);
    void writeSDI(uint8_t *data, uint32_t len);

    // SCI Register R/W

    /**
     * Read a SCI register.
     * @param  addr SCI Register Address.
     * @return      Returns the 16-bit register data.
     */
    uint16_t readREG(uint8_t addr);

    /**
     * Write to a SCI Register.
     * @param addr SCI Register Address to write to.
     * @param reg  Register data.
     */
    void writeREG(uint8_t addr, uint16_t reg);

};

#endif /* VS1053B_HPP_ */

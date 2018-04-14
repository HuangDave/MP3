/*
 * LabUART.hpp
 *
 *  Created on: Mar 9, 2018
 *      Author: huang
 */

#ifndef LABUART_HPP_
#define LABUART_HPP_

#include "LPC17xx.h"

#include "FreeRTOS.h"
#include "queue.h"

class LabUART {

public:

    typedef enum {
        UART_2 = 0,
        UART_3
    } Peripheral;

    typedef enum {
        UART3_TXD3_0_0,     // P0.0  PINSEL0[1:0]   0b10
        UART3_RXD3_0_1,     // P0.1  PINSEL0[3:2]   0b10

        UART2_TXD2_0_10,    // P0.10 PINSEL0[21:20] 0b01
        UART2_RXD2_0_11,    // P0.11 PINSEL0[23:22] 0b01

        //UART3_TXD3_0_25,  // P0.25 PINSEL1[19:18] 0b11    USED FOR LIGHT SENSOR
        UART3_RXD3_0_26,    // P0.26 PINSEL1[21:20] 0b11

        UART2_TXD2_2_8,     // P2.8  PINSEL4[17:16] 0b10
        UART2_RXD2_2_9,     // P2.9  PINSEL4[19:18] 0b10

        UART3_TXD3_4_28,    // P4.28 PINSEL9[25:24] 0b11
        UART3_RXD3_4_29,    // P4.29 PINSEL9[27:26] 0b11
    } UARTn_Pin_ID;

    typedef enum {
        LEN_5_BIT = 0b00,
        LEN_6_BIT = 0b01,
        LEN_7_BIT = 0b10,
        LEN_8_BIT = 0b11
    } WordLength;

    typedef enum {
        STOP_BIT_1 = 0,
        STOP_BIT_2
    } StopBit;


    typedef struct {
        Peripheral peripheral;
        WordLength wordLen;
        StopBit stopBits;
        //uint32_t rxSize;
        //uint32_t txSize;
    } UART_Config;

    typedef union {
        uint8_t bytes;
        struct {
            uint8_t RDR : 1; // receive data ready                  1 = Rx FIFO is not empty
            uint8_t OE  : 1; // overrun error                       1 = OE status active
            uint8_t PE  : 1; // parity error                        1 = PE status active
            uint8_t FE  : 1; // framing error                       1 = FE status active
            uint8_t BI  : 1; // break interrupt                     1 = BI status active
            uint8_t THRE: 1; // transmiter holding register empty   1 = THR empty, 0 = contains valid data
            uint8_t TEMT: 1; // transmiter empty                    1 = TSR empty, 0 = contains valid data
            uint8_t RXFE: 1; // error in RX FIFI                    1 = empty, 0 = no RX errors or FCR[0] = 0
        };
    } Status;

    /**
     * Destructor
     */
    virtual ~LabUART();

    /**
     * Initializes the UART peripheral to be configured to either UART2 or UART3
     *
     * @param config UART configurations
     * @param baud   Desired baud rate for data transfer.
     */
    void init(UART_Config config, uint32_t baud);

    void setBaud(uint32_t rate);

    void selectPin(UARTn_Pin_ID pinId);

    /**
     * Gets the current status of the peripheral from the status register (LSR)
     * @return Returns a struct containing the LSR statuses.
     */
    Status getStatus();

    /**
     * Sends a byte through polling.
     * @param byte Byte to send.
     */
    void send(uint8_t byte);

    /**
     * Queues a byte to be sent through Rx.
     *
     * @param  byte    Byte to add to the queue.
     * @param  timeout [description]
     * @return         returns true if the byte was added to the queue or is sent.
     */
    bool send(uint8_t byte, uint32_t timeout);

    /**
     * Receive a byte through polling.
     * @return Returns the received byte.
     */
    uint8_t receive();

    /**
     * Receives a queued byte.
     *
     * @param  byte    Address to store byte.
     * @param  timeout
     * @return         Returns true if the byte was received.
     */
    bool receive(uint8_t *byte, uint32_t timeout);

    void handleInterrupt();

protected:

    typedef enum {
        TX_QUEUE,
        RX_QUEUE
    } UART_QUEUE;

    /// Base pointer of peripheral.
    volatile LPC_UART_TypeDef *UARTn;

    /// Current peripherl, can be UART2 or UART3
    Peripheral mPeripheral;

    /// Baud rate of UART peripheral
    uint32_t mBaud;

    /// queues for Tx and Rx lines
    QueueHandle_t mQueues[2];

    /**
     * Constructor
     */
    LabUART();

private:

    /// Array consisting of UART base pointers.
    static volatile LPC_UART_TypeDef *UART[];
};

#endif /* LABUART_HPP_ */

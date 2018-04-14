/*
 * LabUART.cpp
 *
 *  Created on: Mar 9, 2018
 *      Author: huang
 */

#include <L5/LabUART.hpp>
#include <stddef.h>
#include <lpc_isr.h>
#include "sys_config.h"
#include "task.h"

#include "printf_lib.h"

volatile LPC_UART_TypeDef* LabUART::UART[] = { LPC_UART2, LPC_UART3 };

LabUART::LabUART() {
    UARTn = NULL;

    mBaud = 0;
    mPeripheral = UART_2;

    mQueues[TX_QUEUE] = 0;
    mQueues[RX_QUEUE] = 0;
}

LabUART::~LabUART() {
    // TODO Auto-generated destructor stub
}

void LabUART::init(UART_Config config, uint32_t baud) {
    mPeripheral = config.peripheral;

    LPC_SC->PCONP |= (mPeripheral == UART_2 ? (1 << 24) : (1 << 25));           // enable PCUART2 or PCUART3

    // enable UARTn peripheral clock..
    LPC_SC->PCLKSEL1 &= (mPeripheral== UART_2 ? ~(3 << 16) : ~(1 << 18));
    LPC_SC->PCLKSEL1 |= (mPeripheral == UART_2 ? (1 << 16) :  (1 << 18));       // enable PCLK with division = 1

    UARTn = UART[mPeripheral];

    setBaud(baud);                                                              // set baud rate

    // configure word length, and number of stop bits...
    UARTn->LCR &= ~(3 << 0);                                                    // set word length 5, 6, 7, or 8 bits
    UARTn->LCR |= (config.wordLen << 0);

    if (config.stopBits == STOP_BIT_1)                                          // set number of STOP bits
        UARTn->LCR &= ~(1 << 2);
    else
        UARTn->LCR |= (1 << 2);

    // initialize Queue and configure FIFO...
    mQueues[TX_QUEUE] = xQueueCreate(8, sizeof(uint8_t));
    mQueues[RX_QUEUE] = xQueueCreate(8, sizeof(uint8_t));

    UARTn->FCR |= (1 << 0);
    UARTn->FCR |= (1 << 1);                                                     // enable FIFO Rx reset

    // enable the peripheral to be interrupt driven...
    UARTn->IER |= (1 << 0);// | (1 << 1);

    switch (mPeripheral) {
        case UART_2: NVIC_EnableIRQ(UART2_IRQn); break;
        case UART_3: NVIC_EnableIRQ(UART3_IRQn); break;
        default: return;
    }
}

void LabUART::setBaud(uint32_t baud) {
    // calculate DL and actual baud rate
    volatile unsigned long pclk = sys_get_cpu_clock();                          // ger peripherl clock based on CPU clock PCLK = CLK / 1

    if (baud == 0) baud = 9600;
    uint16_t dl = pclk / (baud * 16);                                           // calculate 16-bit DL
    mBaud = pclk / (16 * dl);                                                   // calculate actual baud rate in use

    UARTn->LCR |= (1 << 7);                                                     // set DLAB = 1 to enable DLM and DLL
    UARTn->DLM |= ((dl >> 8) & 0xFF);                                           // set hi and lo of DL register
    UARTn->DLL |= ((dl >> 0) & 0xFF);
    UARTn->LCR &= ~(1 << 7);                                                    // set DLAB = 0
}

void LabUART::selectPin(UARTn_Pin_ID pinId) {
    switch (pinId) {
        case UART3_TXD3_0_0:  { LPC_PINCON->PINSEL0 &= ~(3 << 0); LPC_PINCON->PINSEL0 |= (2 << 0); } break;
        case UART3_RXD3_0_1:  { LPC_PINCON->PINSEL0 &= ~(3 << 2); LPC_PINCON->PINSEL0 |= (2 << 2); } break;

        case UART2_TXD2_0_10: { LPC_PINCON->PINSEL0 &= ~(3 << 20); LPC_PINCON->PINSEL0 |= (1 << 20); } break;
        case UART2_RXD2_0_11: { LPC_PINCON->PINSEL0 &= ~(3 << 22); LPC_PINCON->PINSEL0 |= (1 << 22); } break;

        //case UART3_TXD3_0_25: { LPC_PINCON->PINSEL1 |= (3 << 18); } break;
        case UART3_RXD3_0_26: { LPC_PINCON->PINSEL1 |= (3 << 20); } break;

        case UART2_TXD2_2_8: { LPC_PINCON->PINSEL4 &= ~(3 << 16); LPC_PINCON->PINSEL4 |= (2 << 16); } break;
        case UART2_RXD2_2_9: { LPC_PINCON->PINSEL4 &= ~(3 << 18); LPC_PINCON->PINSEL4 |= (2 << 18); } break;

        case UART3_TXD3_4_28: { LPC_PINCON->PINSEL9 |= (3 << 24); } break;
        case UART3_RXD3_4_29: { LPC_PINCON->PINSEL9 |= (3 << 26); } break;
    }
}

LabUART::Status LabUART::getStatus() {
    Status status;
    status.bytes = (UARTn->LSR & 0xFF);
    return status;
}

void LabUART::send(uint8_t byte) {
    UARTn->THR = byte;
    while(!getStatus().THRE);                                                   // wait byte to be sent, THRE to be empty
}

bool LabUART::send(uint8_t byte, uint32_t timeout) {

    if (!xQueueSend(mQueues[TX_QUEUE], &byte, timeout))                         // attempt to queue the byte, return false if queue is full
        return false;

    if (!getStatus().THRE) {                                                    // send queued byte from FIFO if transmiter holding register is idling
        if (xQueueReceive(mQueues[TX_QUEUE], &byte, 0)) {
            UARTn->THR = byte;
        }
    }
    return true;
}

uint8_t LabUART::receive() {
    while(!getStatus().RDR);                                                    // wait for FIFO to be filled to receive byte
    return UARTn->RBR;
}

bool LabUART::receive(uint8_t *byte, uint32_t timeout) {
    return xQueueReceive(mQueues[RX_QUEUE], byte, timeout);                     // dequeue Rx
}

void LabUART::handleInterrupt() {
    uint8_t byte = 0;

    while(getStatus().RDR) {                                                    // receive bytes while Rx FIFO is not empty
         byte = UARTn->RBR;
         xQueueSendFromISR(mQueues[RX_QUEUE], &byte, NULL);
         //xQueueSend(mQueues[RX_QUEUE], &byte, portMAX_DELAY);
    }

    uint8_t clr = UARTn->LSR;                                                   // clear interrupt
}

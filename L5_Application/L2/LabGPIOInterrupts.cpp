/*
 * LabGPIOInterrupts.cpp
 *
 *  Created on: Feb 15, 2018
 *      Author: huang
 */

#include <L2/LabGPIOInterrupts.hpp>
#include <lpc_isr.h>

LabGPIOInterrupts *LabGPIOInterrupts::instance = NULL;

ISRHandler LabGPIOInterrupts::mpHandlers[2][32] = { NULL };

LabGPIOInterrupts::LabGPIOInterrupts() {
    init();
}

LabGPIOInterrupts& LabGPIOInterrupts::sharedInstance() {
    if (instance == NULL)
        instance = new LabGPIOInterrupts();
    return *instance;
}

void LabGPIOInterrupts::init() {

    LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;
    LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;

    NVIC_EnableIRQ(EINT3_IRQn);
    isr_register(EINT3_IRQn, LabGPIOInterrupts::externalIRQHandler);
}

void LabGPIOInterrupts::enableIntTrigger(uint8_t port, uint32_t pin, EIntTrigger condition) {

    // pointers to registers to set and clear rising / falling edge trigger
    volatile uint32_t *pIntEnable;
    volatile uint32_t *pIntDisable;

    switch (port) {
        case 0: {
            pIntEnable  = (condition == RISING_EDGE) ? &(LPC_GPIOINT->IO0IntEnR) : &(LPC_GPIOINT->IO0IntEnF);
            pIntDisable = (condition == RISING_EDGE) ? &(LPC_GPIOINT->IO0IntEnF) : &(LPC_GPIOINT->IO0IntEnR);
        } break;
        case 2: {
            pIntEnable  = (condition == RISING_EDGE) ? &(LPC_GPIOINT->IO2IntEnR) : &(LPC_GPIOINT->IO2IntEnF);
            pIntDisable = (condition == RISING_EDGE) ? &(LPC_GPIOINT->IO2IntEnF) : &(LPC_GPIOINT->IO2IntEnR);
        } break;
        default: return;
    }

    *pIntEnable |= (1 << pin);                      // set triggering register
    if (condition == RISING_FALLING)                // set the opposite register if trigger by both edges
        *pIntDisable |= (1 << pin);
    else                                            // otherwise clear opposite register
        *pIntDisable &= ~(1 << pin);
}

bool LabGPIOInterrupts::attachInterruptHandler(uint8_t port, uint32_t pin,  ISRHandler handler, EIntTrigger condition) {

    if (port == 1 || port > 2) return false;

    enableIntTrigger(port, pin, condition);         // enable interrupt to be rising or falling edge triggered...
    mpHandlers[!!port][pin] = handler;

    return true;
}

bool LabGPIOInterrupts::attachInterruptHandler(LabGPIO gpio,  ISRHandler handler, EIntTrigger condition) {
    return attachInterruptHandler(gpio.getPortNum(), gpio.getPinNum(), handler, condition);
}

void LabGPIOInterrupts::externalIRQHandler(void) {
    for (uint32_t i = 0; i < 32; i++) {
        if (LPC_GPIOINT->IO0IntStatR & (1 << i)) {
            mpHandlers[0][i](0, i);
            LPC_GPIOINT->IO0IntStatR &= ~(1 << i);  // clear rising or falling edge flag
            LPC_GPIOINT->IO0IntClr |= (1 << i);     // clear interrupt flag
        }
        if (LPC_GPIOINT->IO0IntStatF & (1 << i)) {
            mpHandlers[0][i](0, i);
            LPC_GPIOINT->IO0IntStatF &= ~(1 << i);  // clear rising or falling edge flag
            LPC_GPIOINT->IO0IntClr |= (1 << i);     // clear interrupt flag
        }
        if (LPC_GPIOINT->IO2IntStatR & (1 << i)) {
            mpHandlers[1][i](2, i);
            LPC_GPIOINT->IO2IntStatR &= ~(1 << i);  // clear rising or falling edge flag
            LPC_GPIOINT->IO2IntClr |= (1 << i);     // clear interrupt flag
        }
        if (LPC_GPIOINT->IO2IntStatF & (1 << i)) {
            mpHandlers[1][i](2, i);
            LPC_GPIOINT->IO2IntStatF &= ~(1 << i);  // clear rising or falling edge flag
            LPC_GPIOINT->IO2IntClr |= (1 << i);     // clear interrupt flag
        }
    }
}

LabGPIOInterrupts::~LabGPIOInterrupts() {
    // clear all interrupts...
    LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;
    LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
}

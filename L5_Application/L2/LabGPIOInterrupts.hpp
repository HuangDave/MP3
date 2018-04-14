/*
 * LabGPIOInterrupts.hpp
 *
 *  Created on: Feb 15, 2018
 *      Author: huang
 */

#ifndef LABGPIOINTERRUPTS_H
#define LABGPIOINTERRUPTS_H

#include <stddef.h>
#include "LPC17xx.h"
#include "../L1/LabGPIO.hpp"

#define GPIOINT LabGPIOInterrupts::sharedInstance()

typedef void (*ISRHandler)(uint8_t, uint32_t);

class LabGPIOInterrupts {

public:

    typedef enum {
        FALLING_EDGE    = 0,
        RISING_EDGE     = 1,
        RISING_FALLING  = 2
    } EIntTrigger;

    static LabGPIOInterrupts& sharedInstance();

    /**
     * 1) Should setup register "externalIRQHandler" as the EINT3 ISR.
     * 2) Should configure NVIC to notice EINT3 IRQs.
     */
    void init();

    /**
     * This handler should place a function pointer within the lookup table for the externalIRQHandler to find.
     *
     * @param[in] port         specify the GPIO port
     * @param[in] pin          specify the GPIO pin to assign an ISR to
     * @param[in] pin_isr      function to run when the interrupt event occurs
     * @param[in] condition    condition for the interrupt to occur on. RISING, FALLING or BOTH edges.
     * @return should return true if valid ports, pins, isrs were supplied and pin isr insertion was sucessful
     */
    bool attachInterruptHandler(uint8_t port, uint32_t pin, ISRHandler handler, EIntTrigger condition);

    bool attachInterruptHandler(LabGPIO gpio,  ISRHandler handler, EIntTrigger condition);

    /**
     * After the init function has run, this will be executed whenever a proper
     * EINT3 external GPIO interrupt occurs. This function figure out which pin
     * has been interrupted and run the correspondingISR for it using the lookup table.
     *
     * VERY IMPORTANT! Be sure to clear the interrupt flag that caused this
     * interrupt, or this function will be called again and again and again, ad infinitum.
     *
     * Also, NOTE that your code needs to be able to handle two GPIO interrupts occurring
     * at the same time.
     */
    static void externalIRQHandler(void);

    ~LabGPIOInterrupts();

private:
    // Singleton instance
    static LabGPIOInterrupts *instance;

    // LUT of ISR Handlers
    static ISRHandler mpHandlers[2][32];

    /**
     * LabGPIOInterrupts should be a singleton, meaning, only one instance can exist at a time.
     * Look up how to implement this.
     */
    LabGPIOInterrupts();

    /**
     * Set and clear the GPIO interrupt triggers.
     * If the condition is trigger on rising edge, then set rising edge register and clear falling edge register.
     * If following edge triggered then do the opposite.
     * If trigger on both edges then set both registers.
     *
     * @param port      GPIO port.
     * @param pin       GPIO pin for ISR.
     * @param condition condition for intterrupt to occur.
     */
    void enableIntTrigger(uint8_t port, uint32_t pin, EIntTrigger condition);
};

#endif

/*
 * LabGPIO.cpp
 *
 *  Created on: Feb 2, 2018
 *      Author: huang
 */

#include "LabGPIO.hpp"

/// Constructor
LabGPIO::LabGPIO(uint8_t port, uint8_t pin, bool output, bool high) : mPort(port), mPin(pin) {
    // base pointers of ports mapped in memory
    LPC_GPIO_TypeDef *basePorts[] = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4 };
    // store reference to base pointer of the specified port
    mpGPIOPin = basePorts[mPort];

    // set pin function to be GPIO
    volatile uint32_t *pinsel = &(LPC_PINCON->PINSEL0);
    pinsel[2*mPort] &= ~(3 << (2*mPin));

    setDirection(output);
    if (output) set(high);
}

/// Destructor
LabGPIO::~LabGPIO()                     { setLow(); }

uint8_t LabGPIO::getPortNum() const     { return mPort; }
uint8_t LabGPIO::getPinNum()  const     { return mPin; }

void LabGPIO::setDirection(bool output) { output ? setAsOutput() : setAsInput(); }
void LabGPIO::setAsInput()              { mpGPIOPin->FIODIR &= ~(1 << mPin); }
void LabGPIO::setAsOutput()             { mpGPIOPin->FIODIR |=  (1 << mPin); }

void LabGPIO::set(bool high)            { high ? setHigh() : setLow(); }
void LabGPIO::setHigh()                 { mpGPIOPin->FIOSET = (1 << mPin); };
void LabGPIO::setLow()                  { mpGPIOPin->FIOCLR = (1 << mPin); };

/// return false if level is 0 or true if level is >= 1
bool LabGPIO::getLevel()                { return !!( mpGPIOPin->FIOPIN & (1 << mPin) ); }

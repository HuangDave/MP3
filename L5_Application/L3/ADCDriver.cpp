/*
 * ADCDriver.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: huang
 */

#include <L3/ADCDriver.hpp>
#include "LPC17xx.h"
#include "sys_config.h"

ADCDriver* ADCDriver::instance = NULL;

ADCDriver& ADCDriver::sharedInstance() {
    if (instance == NULL)
        instance = new ADCDriver();
    return *instance;
}

ADCDriver::ADCDriver() {
    adcInitBurstMode();
}

void ADCDriver::adcInitBurstMode() {
    LPC_SC->PCONP |= (1 << 12);                             // power up PCADC

    LPC_SC->PCLKSEL0 &= ~(3 << 24);                         // clear enable PCLK_ADC with CLKDIV 8
    LPC_SC->PCLKSEL0 |= (3 << 24);                          // enable PCLK

    LPC_ADC->ADCR |= (1 << 8) | (1 << 21) | (1 << 16);      // set APB CLKDIV = 1, enable ADC, enable BURST
}

void ADCDriver::adcSelectPin(ADC_PIN pinId) {
    switch (pinId) {                                        // clear and set PINSEL function for ADC0.x as needed
        case ADC_PIN_0_26: { LPC_PINCON->PINSEL1 &= ~(3 << 20);
                             LPC_PINCON->PINSEL1 |= (1 << 20); } break;
        case ADC_PIN_1_30:   LPC_PINCON->PINSEL3 |= (3 << 28); break;
        case ADC_PIN_1_31:   LPC_PINCON->PINSEL3 |= (3 << 30); break;
        default: return;                                    // do not configure the other pins
    }
}

float ADCDriver::readADCVoltageByChannel(uint8_t channel) {
    LPC_ADC->ADCR &= ~(0xFF << 0);                          // clear SEL channel
    LPC_ADC->ADCR |= (1 << channel);
    const float vRef = 3.3;                                 // reference voltage
    const float res = 4096.0;                               // resolution
    return ((LPC_ADC->ADGDR >> 4) & 0xFFF) * (vRef/res);
}

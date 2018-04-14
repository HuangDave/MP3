/*
 * PWMDriver.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: huang
 */

#include <L3/PWMDriver.hpp>
#include "LPC17xx.h"
#include "sys_config.h"

PWMDriver* PWMDriver::instance = NULL;

uint32_t PWMDriver::mCT = 0;

PWMDriver& PWMDriver::sharedInstance() {
    if (instance == NULL)
        instance = new PWMDriver();
    return *instance;
}

PWMDriver::PWMDriver() {
    pwmInitSingleEdgeMode(100);                             // initialize with 100Hz
}

void PWMDriver::pwmSelectAllPins() {
    LPC_PINCON->PINSEL4 &= ~(0xFFF);
    LPC_PINCON->PINSEL4 |= (0x555);
}

void PWMDriver::pwmSelectPin(PWM_PIN pin) {
    LPC_PINCON->PINSEL4 &= ~(3 << (2*pin));                 // set function to PWM1
    LPC_PINCON->PINSEL4 |= (1 << (2*pin));
}

void PWMDriver::pwmInitSingleEdgeMode(uint32_t frequency_Hz) {
    LPC_SC->PCONP |= (1 << 6);                              // enable PCPWM1

    LPC_SC->PCLKSEL0 &= ~(3 << 12);                         // enable pclk for PCLK_PWM1
    LPC_SC->PCLKSEL0 |= (1 << 12);

    LPC_PWM1->PR = 0;                                       // no prescalar
    LPC_PWM1->CTCR &= ~(0xF);                               // set TC to increment when PC matches PR
    LPC_PWM1->CCR |= 0x3F;                                  // disable capture features
    LPC_PWM1->MCR |= (1 << 1);                              // configure counter to reset when MR0 is matched

    setFrequency(frequency_Hz);                             // set initial desired frequency of 100Hz

    LPC_PWM1->TCR = (1 << 0) | (1 << 3);                    // enable timer and prescalar counter and PWM mode
    LPC_PWM1->PCR |= (0x3F << 9);                           // enable all 6 PWMENAx outputs, reg 9-14
    LPC_PWM1->PCR &= ~(0x1F << 2);                          // enable single edge on all channels, first 2 bits unused

    for (int i = PWM_PIN_2_0; i < PWM_PIN_2_5; i++)         // set zero duty cycle for all channels
        setDutyCycle((PWM_PIN)i, 0);
}

void PWMDriver::setDutyCycle(PWM_PIN pin, float duty_cycle_percentage) {
    // clamp percentage to 0.0 or 1.0 if needed
    if (duty_cycle_percentage < 0)      duty_cycle_percentage = 0.0;
    else if (duty_cycle_percentage > 1) duty_cycle_percentage = 1.0;

    const unsigned int out = mCT * duty_cycle_percentage;

    switch (pin) {
        case PWM_PIN_2_0: LPC_PWM1->MR1 = out; break;
        case PWM_PIN_2_1: LPC_PWM1->MR2 = out; break;
        case PWM_PIN_2_2: LPC_PWM1->MR3 = out; break;
        case PWM_PIN_2_3: LPC_PWM1->MR4 = out; break;
        case PWM_PIN_2_4: LPC_PWM1->MR5 = out; break;
        case PWM_PIN_2_5: LPC_PWM1->MR6 = out; break;
        default: return;
    }
    LPC_PWM1->LER |= (1 << (pin+1));                        // enable latch
}

void PWMDriver::setFrequency(uint32_t frequency_Hz){
    if (frequency_Hz == 0)                                  // if freq is 0, default to 100Hz
        frequency_Hz = 100;
    volatile unsigned long cpuClk = sys_get_cpu_clock();    // should be 48MHz
    mCT = (cpuClk / frequency_Hz);
    LPC_PWM1->MR0 = mCT;                                    // set freq
}

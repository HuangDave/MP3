/*
 * PWMDriver.hpp
 *
 *  Created on: Feb 23, 2018
 *      Author: huang
 */

#ifndef PWM_DRIVER_H_
#define PWM_DRIVER_H_

#include "stdint.h"
#include <stddef.h>

#define PWM PWMDriver::sharedInstance()

class PWMDriver {

private:

    static PWMDriver *instance;

    // max count for MR
    static uint32_t mCT;

    /**
    * Nothing needs to be done within the default constructor
    */
    PWMDriver();

public:

    enum PWM_PIN {
        PWM_PIN_2_0 = 0,    // PWM1.1
        PWM_PIN_2_1,        // PWM1.2
        PWM_PIN_2_2,        // PWM1.3
        PWM_PIN_2_3,        // PWM1.4
        PWM_PIN_2_4,        // PWM1.5
        PWM_PIN_2_5         // PWM1.6
    };

    static PWMDriver& sharedInstance();

    /**
    * 1) Power up the PWM peripheral
    * 2) Set the PWM clock to divide by 1 (to simplify frequency calculation
    * 3) Enable timer and prescalar counters. Enable PWM Mode
    * 4) Configure Counter to reset when MR0 is matched (i.e. MR0 represents frequency)
    * 5) Disable all capture features
    * 6) Enable Timer mode and disable counter/capture modes
    * 7) Enable single edge mode
    * 8) Enable output on all six pwm channels
    * 9) Set frequency
    * 10) Set all pwm channels to zero duty cycle
    *
    * @param frequency_Hz is the initial frequency in Hz.
    */
    void pwmInitSingleEdgeMode(uint32_t frequency_Hz);

    /**
     * Power off the PWM peripheral
     */
    void powerOff();

    /**
    * 1) Select PWM functionality on all PWM-able pins.
    */
    void pwmSelectAllPins();

    /**
    * 1) Select PWM functionality of pwm_pin_arg
    *
    * @param pwm_pin_arg is the PWM_PIN enumeration of the desired pin.
    */
    void pwmSelectPin(PWM_PIN pin);

    /**
    * 1) Convert duty_cycle_percentage to the appropriate match register value (depends on current frequency)
    * 2) Assign the above value to the appropriate MRn register (depends on pwm_pin_arg)
    *
    * @param pwm_pin_arg is the PWM_PIN enumeration of the desired pin.
    * @param duty_cycle_percentage is the desired duty cycle percentage from 0.0 - 1.0
    */
    void setDutyCycle(PWM_PIN pwm_pin_arg, float duty_cycle_percentage);

    /**
    * 1) Convert frequency_Hz to the appropriate match register value
    * 2) Assign the above value to MR0
    *
    * @param frequency_hz is the desired frequency of all pwm pins
    */
    void setFrequency(uint32_t frequency_Hz);
};

#endif

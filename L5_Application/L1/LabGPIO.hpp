#ifndef LABGPIO_H
#define LABGPIO_H

#include "LPC17xx.h"

class LabGPIO {

protected:

    /// reference to the base pointer of the GPIO port mapped in memory
    volatile LPC_GPIO_TypeDef *mpGPIOPin;
    /// GPIO port number
    uint8_t mPort;
    /// GPIO pin number
    uint8_t mPin;

public:

    /**
     * You should not modify any hardware registers at this point
     * You should store the port and pin using the constructor.
     *
     * @param {uint8_t} port - port number between 0 and 1
     * @param {uint8_t} pin  - pin number between 0 and 32
     */
    LabGPIO(uint8_t port, uint8_t pin);

    LabGPIO(uint8_t port, uint8_t pin, bool output, bool high);

    uint8_t getPortNum() const;

    uint8_t getPinNum() const;

    /**
     * Should alter the hardware registers to set the pin as an input
     */
    void setAsInput();

    /**
     * Should alter the hardware registers to set the pin as an input
     */
    void setAsOutput();

    /**
     * Should alter the set the direction output or input depending on the input.
     *
     * @param {bool} output - true => output, false => set pin to input
     */
    void setDirection(bool output);

    /**
     * Should alter the hardware registers to set the pin as high
     */
    void setHigh();

    /**
     * Should alter the hardware registers to set the pin as low
     */
    void setLow();

    /**
     * Should alter the hardware registers to set the pin as low
     *
     * @param {bool} high - true => pin high, false => pin low
     */
    void set(bool high);

    /**
     * Should return the state of the pin (input or output, doesn't matter)
     *
     * @return {bool} level of pin high => true, low => false
     */
    bool getLevel();

    ~LabGPIO();
};

#endif

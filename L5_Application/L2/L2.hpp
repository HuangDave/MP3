
#include "tasks.hpp"

#include "FreeRTOS.h"
#include "printf_lib.h"
#include "utilities.h"

#include "../L1/LabGPIO.hpp"
#include "LabGPIOInterrupts.hpp"

typedef enum {
    SW1 = 0,
    SW2 = 1
} SwitchID;

static LabGPIO switches[]   = { LabGPIO(2, 1), LabGPIO(2, 2) }; // array containing gpio switches used for gpio interrupts
static LabGPIO leds[]       = { LabGPIO(2, 3), LabGPIO(2, 4) }; // array containing both the LEDs to display gpio interrupt callback

SemaphoreHandle_t semaphores[2];                                // Two semaphores, one for internal and one for external SW and LED tasks

void controlLED(uint8_t port, uint32_t pin) {
            u0_dbg_printf("%d %d\n", port, pin);

    if ((port == 2 && pin == 1) || (port == 2 && pin == 2)) {
        SwitchID switchID = (port == 2 && pin == 1) ? SW1 : SW2;
        if (xSemaphoreTake( semaphores[switchID], portMAX_DELAY) ) {
            leds[switchID].set( !leds[switchID].getLevel() );
            xSemaphoreGive( semaphores[switchID] );
        }
    }
};

int L2() {
    // set output directions of for LEDs...
    leds[SW1].setAsOutput();
    leds[SW2].setAsOutput();

    // initialize semaphores and create RTOS tasks to read the switches and control the LEDs...
    for (int i = 0; i < 2; i ++)
        semaphores[i] = xSemaphoreCreateMutex();

    // setup internal switch to be triggered on falling clock edge
    GPIOINT.attachInterruptHandler(switches[SW1], &controlLED, LabGPIOInterrupts::FALLING_EDGE);
    GPIOINT.attachInterruptHandler(switches[SW2], &controlLED, LabGPIOInterrupts::RISING_FALLING);

    while (1) delay_ms(100);

    return 0;
};

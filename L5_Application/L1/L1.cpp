/*
#include "tasks.hpp"
#include "examples/examples.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include "uart0_min.h"

#include "LabGPIO.hpp"

typedef enum {
    INTERNAL_SWITCH_ID = 0,
    EXTERNAL_SWITCH_ID = 1
} SwitchID;

/// array containing both the internal and external switches
static LabGPIO switches[]   = { LabGPIO(1, 9), LabGPIO(1, 19) };
/// array containing both the internal and external LEDs
static LabGPIO leds[]       = { LabGPIO(1, 0), LabGPIO(1, 20)  };

static bool toggleInternalLED = false;
static bool toggleExternalLED = false;

/// Two semaphores, one for internal and one for external SW and LED tasks
SemaphoreHandle_t semaphores[2];

void vControlLED(void *pvParameters) {
    uint32_t ledID = (uint32_t)(pvParameters);

    while (1) {
        // toggle external LED if external switch was released and toggleExternalLED flag is true...
        if (ledID == INTERNAL_SWITCH_ID && toggleInternalLED) {
            if (semaphores[ledID] != NULL && xSemaphoreTake( semaphores[ledID], portMAX_DELAY ) == pdTRUE) {    // wait
                toggleInternalLED = false;
                leds[INTERNAL_SWITCH_ID].set( !leds[INTERNAL_SWITCH_ID].getLevel() );
                xSemaphoreGive( semaphores[ledID] );                                                            // signal
            }

        // toggle external LED if external switch was released and toggleExternalLED flag is true...
        } else if (ledID == INTERNAL_SWITCH_ID && toggleExternalLED) {
            if (semaphores[ledID] != NULL && xSemaphoreTake( semaphores[ledID], portMAX_DELAY ) == pdTRUE) {    // wait
                toggleExternalLED = false;
                leds[EXTERNAL_SWITCH_ID].set( !leds[EXTERNAL_SWITCH_ID].getLevel() );
                xSemaphoreGive( semaphores[ledID] );                                                            // signal
            }
        }
    }
    vTaskDelete(NULL);
}

void vReadSwitch(void *pvParameters) {
    uint32_t switchID = (uint32_t)(pvParameters);

    while (1) {

        // detect if switch is pressed/held...
        if (switches[switchID].getLevel()) {
            while(switches[switchID].getLevel());

            // raise flag to toggle a corresponding LED when the switch is released...
            if (semaphores[switchID] != NULL && xSemaphoreTake( semaphores[switchID], portMAX_DELAY ) == pdTRUE) {
                if (switchID == INTERNAL_SWITCH_ID)
                    toggleInternalLED = true;
                else if (switchID == EXTERNAL_SWITCH_ID)
                    toggleExternalLED = true;

                xSemaphoreGive( semaphores[switchID] );
            }
        }
    }
    vTaskDelete(NULL);
}

int main(int argc, char const *argv[]) {

    const uint32_t STACK_SIZE = 1024;

    // set input and output directions of switches and LEDs...
    switches[INTERNAL_SWITCH_ID].setAsInput();
    leds[INTERNAL_SWITCH_ID].setAsOutput();

    switches[EXTERNAL_SWITCH_ID].setAsInput();
    leds[EXTERNAL_SWITCH_ID].setAsOutput();

    // initialize semaphores and create RTOS tasks to read the switches and control the LEDs...
    for (int i = 0; i < 2; i ++)
        semaphores[i] = xSemaphoreCreateMutex();

    // create tasks to read internal switch #1 and control internal LED #1...
    xTaskCreate(vReadSwitch, "read_int_switch", STACK_SIZE, (void *)INTERNAL_SWITCH_ID, 1, 0 );
    xTaskCreate(vControlLED, "ctrl_int_led",    STACK_SIZE, (void *)INTERNAL_SWITCH_ID, 1, 0 );

    // create tasks to read external switch and control external LED...
    xTaskCreate(vReadSwitch, "read_ext_switch", STACK_SIZE, (void *)EXTERNAL_SWITCH_ID, 1, 0 );
    xTaskCreate(vControlLED, "ctrl_ext_led",    STACK_SIZE, (void *)EXTERNAL_SWITCH_ID, 1, 0 );

    vTaskStartScheduler();

    return 0;
}
*/

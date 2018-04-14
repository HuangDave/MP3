/*
 * L6.hpp
 *
 *  Created on: Apr 4, 2018
 *      Author: huang
 */

#include "printf_lib.h"
#include "utilities.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "io.hpp"

typedef enum {
    Invalid,
    Up,
    Down,
    Left,
    Right,
    Neutral
} Orientation;

static QueueHandle_t queue = xQueueCreate(5, sizeof(Orientation));

bool between(int16_t val, int16_t lower, int16_t upper) {
    return (val > lower && val < upper);
}

void vProducer(void *params) {
    while (1) {
        int16_t x = AS.getX();
        int16_t y = AS.getY();
        //int16_t z = AS.getZ();

        // determine orientation
        Orientation orientation = Invalid;
        if (x < -300 && between(y, 0, 200))         orientation = Right;
        else if (x > 300 && between(y, 0, 200))     orientation = Left;
        else if (between(x, -100, 100) && y < -300) orientation = Up;
        else if (between(x, -100, 100) && y > 300)  orientation = Down;
        else if (between(x, -100, 100) && between(y, -200, 200)) orientation = Neutral;

        u0_dbg_printf("sending\n");
        xQueueSend(queue, &orientation, 0);
        u0_dbg_printf("sent\n");

        vTaskDelay(1000);
    }
}

void vConsumer(void *params) {
    while (1) {
        Orientation orientation = Invalid;

        u0_dbg_printf("waiting\n");
        xQueueReceive(queue, &orientation, portMAX_DELAY);
        u0_dbg_printf("received\n");

        switch (orientation) {
            case Invalid:   u0_dbg_printf("    Invalid\n"); break;
            case Left:      u0_dbg_printf("    Left\n"); break;
            case Right:     u0_dbg_printf("    Right\n"); break;
            case Up:        u0_dbg_printf("    Up\n"); break;
            case Down:      u0_dbg_printf("    Down\n"); break;
            case Neutral:   u0_dbg_printf("    Neutral\n"); break;
        }
    }
}

int L6() {

    const uint32_t STACK_SIZE = 1024;
    xTaskCreate(vProducer, "produce", STACK_SIZE, (void *) 0, 1, NULL);
    xTaskCreate(vConsumer, "consume", STACK_SIZE, (void *) 0, 1, NULL);
    vTaskStartScheduler();

    return 0;
}

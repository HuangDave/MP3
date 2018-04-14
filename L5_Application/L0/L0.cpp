
/*
#include "tasks.hpp"
#include "examples/examples.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include "uart0_min.h"

void vTaskOneCode(void *)
{
    while(1)
    {
        uart0_puts("aaaaaaaaaaaaaaaaaaaa");
        vTaskDelay(100); // This sleeps the task for 100ms (because 1 RTOS tick = 1 millisecond)
    }
}

// Create another task and run this code in a while(1) loop
void vTaskTwoCode(void *)
{
    while(1)
    {
        uart0_puts("bbbbbbbbbbbbbbbbbbbb");
        vTaskDelay(100);
    }
}

// You can comment out the sample code of lpc1758_freertos project and run this code instead
int main(int argc, char const *argv[])
{
    /// This "stack" memory is enough for each task to run properly
    const uint32_t STACK_SIZE = 1024;

    // 1) Same Priority: A = 1, B = 1       bbbbaaaa
    // 2) Different Priority: A = 2, B = 1 prints a's then b's
    // 3) Different Priority: A = 1, B = 2 prints b's then a's

    xTaskCreate(vTaskOneCode, "task1", STACK_SIZE, (void *) 0, 1, NULL );
    xTaskCreate(vTaskTwoCode, "task2", STACK_SIZE, (void *) 0, 1, NULL );

    // Start Scheduler - This will not return, and your tasks will start to run their while(1) loop
    vTaskStartScheduler();

    return 0;
} */


///*

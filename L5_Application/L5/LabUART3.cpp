/*
 * LabUART3.cpp
 *
 *  Created on: Mar 16, 2018
 *      Author: huang
 */

#include <L5/LabUART3.hpp>

LabUART3* LabUART3::instance = NULL;

LabUART3& LabUART3::sharedInstance() {
    if (instance == NULL)
        instance = new LabUART3();
    return *instance;
}

LabUART3::LabUART3()
{
    init(UART_Config { UART_3, LEN_8_BIT, STOP_BIT_1 }, 9600);
}

LabUART3::~LabUART3()
{
    // TODO Auto-generated destructor stub
}

extern "C" void UART3_IRQHandler(void) {
    UART3.handleInterrupt();
}

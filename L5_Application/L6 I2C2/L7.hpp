/*
 * L7.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: huang
 */



#include "printf_lib.h"
#include "utilities.h"
#include "io.hpp"

#include "L5/LabUART3.hpp"

typedef enum {
    ADD = 0,
    SUB,
    MUL
} Operator;

typedef enum {
    Sel_Operand1,
    Sel_Operand2,
    Sel_Operator,
    Wait,
    Done
} State;

static State state = Sel_Operand1; // current state of producer
static uint8_t a = 0;
static uint8_t b = 0;
static Operator op = ADD;

uint32_t calculateResult(uint8_t operand) {
    switch (operand) {
        case '+': return (a - '0') + (b - '0');
        case '-': return (a - '0') - (b - '0');
        case '*': return (a - '0') * (b - '0');
    }
    return 0;
}

/**
 * Increment the operator or operand.
 */
void increment() {
    switch (state) {
        case Sel_Operand1: {
            a++; a %= 10; // increment a, if a > 9 reset a to 0
            LD.setNumber(a);
        } break;

        case Sel_Operand2: {
            b++; b %= 10; // increment b, if b > 9 reset b to 0
            LD.setNumber(b);
        } break;

        case Sel_Operator: {
            switch (op) {
                case ADD: op = SUB; break;
                case SUB: op = MUL; break;
                case MUL: op = ADD; break;
            }
            LD.setNumber((uint8_t)op);
        } break;

        default: break;
    }
}

/**
 * Decrement the operator or operand.
 */
void decrement() {
    switch (state) {
        case Sel_Operand1: {    // if a < 0, set a to 9
            if (a == 0) {
                a = 9;
                LD.setNumber(9);
            }
            else {
                LD.setNumber(--a);
            }
        } break;

        case Sel_Operand2: {    // if b < 0, set b to 9
            if (b == 0) {
                b = 9;
                LD.setNumber(9);
            }
            else {
                LD.setNumber(--b);
            }
        } break;

        case Sel_Operator: {
            switch (op) {
                case ADD: op = MUL; break;
                case SUB: op = ADD; break;
                case MUL: op = SUB; break;
            }
            LD.setNumber((uint8_t)op);
        } break;

        default: break;
    }
}

void send() {
    u0_dbg_printf("a: %d\n", a);
    UART3.send(a + '0');

    u0_dbg_printf("b: %d\n", b);
    UART3.send(b + '0');

    uint8_t operand = 0;
    switch (op) {
        case ADD: operand = '+';  break;
        case SUB: operand = '-';  break;
        case MUL: operand = '*';  break;
    }
    u0_dbg_printf("operand: %c\n", operand);
    UART3.send(operand);
}

void reset() {
    LD.setNumber(0);
    a = 0;
    b = 0;
    op = ADD;
    state = Sel_Operand1;
}

/**
 * Go to next state.
 * If next state is Sel_Operandx or Sel_Operator, then reset the 7seg LED to 0
 */
void nextState() {
    switch (state) {
        case Sel_Operand1: {
            LD.setNumber(b);
            state = Sel_Operand2;
        } break;
        case Sel_Operand2: {
            LD.setNumber((uint8_t)op);
            state = Sel_Operator;
        } break;
        case Sel_Operator:  state = Wait; break;
        case Wait:          state = Done; break;
        case Done:          return reset();
    }
}

/**
 * When the board is running as a producer, the producer gets inputs from the user and
 * sends the inputs to the consumer then receives the computed result.
 *
 * The producer has 5 states, the first 3 states gets the input operators and operands from the user.
 *
 * While in the first 3 states: Sel_Operator or Sel_Operand, the user can
 * select the inputs by using Button 1 and 2 on the board to increment or decrement the operator/operand.
 *
 * The 4th states (Wait) waits for the computed results.
 * The producer goes to the 5th state (Done) when the result is received and displays it on the 7seg LED.
 */
void vProduce(void *) {

    while (1) {
        if (state != Wait) {
            if (SW.getSwitch(1) && state != Done) {
                increment();

            } else if (SW.getSwitch(2) && state != Done) {
                decrement();

            } else if (SW.getSwitch(3)) { // send the operators and operand and wait for result
                if (state == Sel_Operator) send(); // if operator is selected, send the operands and operators
                nextState();

            } else if (SW.getSwitch(4)) { // reset
                reset();
            }
        } else { // Wait for result...
            u0_dbg_printf("waiting...\n");
            uint8_t tens = 0, ones = 0;

            if (UART3.receive(&tens, 100)) { // result received...
                UART3.receive(&ones, 100);

                uint32_t result = tens * 10 + ones;
                LD.setNumber(result);
                u0_dbg_printf("result: %d\n", result);
                nextState();
            }
        }
        vTaskDelay(100);
    }
}

/**
 * When the board is running as a consumer, the consumer receives the 2 operands and operators from
 * the producer and then sends the corresponding computed result.
 */
void vConsume(void *) {
    while(1) {

        uint8_t operand = 0;

        if (UART3.receive(&a, 100)) { // get the 2nd operand and operator after 1st operator is received...
            UART3.receive(&b, 100);
            UART3.receive(&operand, 100);

            u0_dbg_printf("a: %c\n", a);
            u0_dbg_printf("b: %c\n", b);
            u0_dbg_printf("op: %c\n", operand);

            // calculate and send result
            uint32_t result = calculateResult(operand);

            u0_dbg_printf("result: %d\n", result);
            LD.setNumber(result);

            // split result into two digits: tens and ones and send both digits
            UART3.send(result / 10);
            UART3.send(result % 10);
        }
    }
}

int L7() {

    UART3.selectPin(LabUART::UART3_TXD3_0_0);
    UART3.selectPin(LabUART::UART3_RXD3_0_1);

    reset();

    const uint32_t STACK_SIZE = 1024;

    // run as consumer or producer...
    xTaskCreate(vProduce, "produce", STACK_SIZE, (void *) 0, 1, NULL );
    //xTaskCreate(vConsume, "consume", STACK_SIZE, (void *) 0, 1, NULL );

    vTaskStartScheduler();

    return 0;
}

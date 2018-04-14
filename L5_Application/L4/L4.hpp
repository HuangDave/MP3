/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include "tasks.hpp"
#include "FreeRTOS.h"
#include "printf_lib.h"
#include "utilities.h"

#include "L4/AdestoFlash.hpp"

void vReadSig(void *params) {
    while (1) {
        AdestoFlash::DeviceInfo info = Flash.getDeviceInfo();
        uint8_t manufacturerId = 0x1F;
        uint16_t deviceId = 0x2600;

        if (info.manufacturerId != manufacturerId || info.deviceId != deviceId) {
            u0_dbg_printf("\n\nOoops... race condition");
            vTaskSuspend(NULL); // Suspend this task
        }

        vTaskDelay(100);
    }
};

void vReadPage(void *params) {
    while (1) {
        Flash.read(AdestoFlash::PageAddress { 0x00, 0x00 });

        vTaskDelay(100);
    }
};

void printFlashStatus() {

    AdestoFlash::StatusRegister status = ((AdestoFlash &)(AdestoFlash::sharedInstance())).getStatus();
    u0_dbg_printf("\nDevice is %s.\n", status.READY ? "ready" : "busy");
    u0_dbg_printf("Most recent main memory page and buffer %s.\n", status.COMP ? "match" : "do not match");
    u0_dbg_printf("Device density: %d\n", status.DENSITY);
    u0_dbg_printf("Sector protection is %s.\n", status.PROTECT ? "enabled" : "disabled");
    u0_dbg_printf("Buffer and page size configured to use %s.\n", status.PG_SIZE ? "\"power of 2\" binary page size" : "standard DataFlash page size");
    u0_dbg_printf("Device is %s.\n", status.READY2 ? "ready" : "busy");
    u0_dbg_printf("Erase/Program operation was %s.\n", status.EPE ? "not successful, an error was detected" : "successful");
    u0_dbg_printf("Sector lockdown is %s.\n", status.SLE ? "enabled" : "disabled");
    u0_dbg_printf("%s.\n", status.PS2 ? "A sector program was suspended while using buffer 2" : "No program was suspended when using buffer 2");
    u0_dbg_printf("%s.\n", status.PS1 ? "A sector program was suspended while using buffer 1" : "No program was suspended when using buffer 1");
    u0_dbg_printf("Sector erase was %s.\n\n", status.ES ? "suspended" : "not suspended");
};

void testRaceCondition() {
    const uint32_t STACK_SIZE = 1024;
    xTaskCreate(vReadSig, "vReadSig", STACK_SIZE, (void *) 0, 1, NULL );
    xTaskCreate(vReadPage, "vReadPage", STACK_SIZE, (void *) 0, 1, NULL );
    vTaskStartScheduler();
};

void printMBR() {
    AdestoFlash::MBR mbr = Flash.readMasterBootRecord();

    u0_dbg_printf("\n");

    u0_dbg_printf("%02x ", mbr.boot_signature);

    u0_dbg_printf("\n");

    for (int i = 511; i > (511-446); i--) {
        u0_dbg_printf("%x", mbr.bytes[i]);
    }
    u0_dbg_printf("\n");

    for (int i = 445; i > 0; i--) {
        u0_dbg_printf("%x", mbr.code[i]);
    }
    u0_dbg_printf("\n");
    for (int i = 511; i >= 0; i--) {
        u0_dbg_printf("%02x ", mbr.bytes[i]);
        if (i % 16 == 0)
            u0_dbg_printf("\n");
    }

    // output master boot record information

    // output boot signature
    u0_dbg_printf("\nBoot Signture: %x\n", mbr.boot_signature);

    // output bootstrap code 446 bytes
    //const uint32_t BOOTSTRAP_CODE_LENGTH = 446;
    u0_dbg_printf("Bootstrap / Executable Code: ");
    for (int i = 0; i < 445; i++)
        u0_dbg_printf("%x", mbr.code[i]);
};

void L4() {

        // Print device info...
        AdestoFlash::DeviceInfo info = Flash.getDeviceInfo();
        u0_dbg_printf("Manufacturer ID: %x\n", info.manufacturerId);
        u0_dbg_printf("Device ID: %x\n", info.deviceId);

        printFlashStatus();

        u0_dbg_printf("\n");

        //printMBR();

        //testRaceCondition();
};

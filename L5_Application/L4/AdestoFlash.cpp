/*
 * AdestoFlash.cpp
 *
 *  Created on: Mar 2, 2018
 *      Author: huang
 */

#include <L1/LabGPIO.hpp>
#include <L4/AdestoFlash.hpp>
#include <printf_lib.h>
#include <stddef.h>
#include <sys/_stdint.h>

#define FLASH_PAGE_SIZE 512 // 512 bytes

AdestoFlash* AdestoFlash::instance;

AdestoFlash& AdestoFlash::sharedInstance() {
    if (instance == NULL)
        instance = new AdestoFlash();
    return *instance;
}

AdestoFlash::AdestoFlash() {
    init(SSP1, DATASIZE_8_BIT, FRAMEMODE_SPI, PCLK_DIV_8);
    mpCS = configureGPIO(0, 6, true, true);
    waitIfBusy();
}

AdestoFlash::DeviceInfo AdestoFlash::getDeviceInfo() {

    DeviceInfo info;
    selectCS();
    {
        transfer(0x9F);                         // send opcode
        info.manufacturerId = transfer(0x00);   // manufacturer ID
        info.deviceIdH = transfer(0x00);        // device ID byte 1
        info.deviceIdL = transfer(0x00);        // device ID byte 2
    }
    deselectCS();

    waitIfBusy();

    return info;
}

AdestoFlash::StatusRegister AdestoFlash::getStatus() {
    StatusRegister status;

    selectCS();
    {
        transfer(0xD7);
        status.StatusH = transfer(0x00);
        status.StatusL = transfer(0x00);
    }
    deselectCS();

    return status;
}

void AdestoFlash::waitIfBusy() {
    while (getStatus().READY == 0);
}

uint8_t* AdestoFlash::read(PageAddress addr) {
    uint8_t *data = new uint8_t[FLASH_PAGE_SIZE];

    selectCS();
    {
        transfer(0xD2);                                 // read from main page
        transfer(addr.byte2);
        transfer(addr.byte1);
        transfer(addr.byte0);
        transfer(0x00);                                 // send 4 dummy bytes to initialize read
        transfer(0x00);
        transfer(0x00);
        transfer(0x00);
        for (uint32_t i = 0; i < FLASH_PAGE_SIZE; i++)  // send dummy bytes
            data[i] = transfer(0x00);
    }
    deselectCS();

    return data;
}

AdestoFlash::MBR AdestoFlash::readMasterBootRecord() {
    uint8_t *data = read(PageAddress { 0x00, 0x00 });
    MBR mbr;
    for (int i = 0; i < FLASH_PAGE_SIZE; i++)
        mbr.bytes[i] = data[FLASH_PAGE_SIZE - 1 - i];

    for (int i = 0; i < 512; i++)
        u0_dbg_printf("%02x ", mbr.bytes[i]);
    u0_dbg_printf("\n");

    return mbr;
}

AdestoFlash::FAT12 AdestoFlash::readBootSector(PageAddress addr) {
    uint8_t *data = read(addr);
    FAT12 sector;
    for (int i = 0; i < FLASH_PAGE_SIZE; i++)
        sector.bytes[i] = data[FLASH_PAGE_SIZE - 1 - i];
    return sector;
}

void AdestoFlash::write(PageAddress addr, uint8_t *data, uint32_t length) {

    selectCS();
    {
        transfer(0x85);                             // write through buffer 1 to main memory
        transfer(addr.byte2);
        transfer(addr.byte1);
        transfer(addr.byte0);
        for (uint32_t i = 0; i < length; i++) {     // send data
            transfer(data[i]);
        }
    }
    deselectCS();

    waitIfBusy();                               // let internal write to main memory complete
}

bool AdestoFlash::erase(uint16_t addr) {
    PageAddress pgAddr = { (addr & 0xFFF), 0x000 } ;

    selectCS();
    {
        transfer(0x81);                             // write through buffer 1 to main memory
        transfer(pgAddr.byte2);
        transfer(pgAddr.byte1);
        transfer(pgAddr.byte0);
    }
    deselectCS();

    waitIfBusy();                               // let internal process run

    if (getStatus().EPE)                        // return false if EPE (Erase/Program Error) flag is raised
        return false;

    return true;
}

void AdestoFlash::chipErase() {

    selectCS();
    {
        transfer(0xC7);
        transfer(0x94);
        transfer(0x80);
        transfer(0x9A);
    }
    deselectCS();

    u0_dbg_printf("Erasing sectors, this may take a while...\n");
    waitIfBusy();                               // let internal process run
    u0_dbg_printf("Erase Complete!\n");
}

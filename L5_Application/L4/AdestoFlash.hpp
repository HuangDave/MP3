/*
 * AdestoFlash.hpp
 *
 *  Created on: Mar 2, 2018
 *      Author: huang
 */

#ifndef ADESTOFLASH_HPP_
#define ADESTOFLASH_HPP_

#include "L4/LabSPI.hpp"

#define Flash AdestoFlash::sharedInstance()

class AdestoFlash: public LabSPI {

private:

    static AdestoFlash *instance;

    /**
     * Wait if the READY flag in status register is 0 to let internal Flash operation to complete.
     */
    void waitIfBusy();

    AdestoFlash();

    ~AdestoFlash();

public:

    /// 16-bit flash status register
    typedef union {
        uint16_t bytes;
        struct {
            uint8_t StatusL;
            uint8_t StatusH;
        };
        struct {
            uint8_t ES:         1;  // Erase Suspend, 1 if a sector is erase suspended
            uint8_t PS1:        1;  // Program Suspend Status (Buffer 1)
            uint8_t PS2:        1;  // Program Suspend Status (Buffer 2)
            uint8_t SLE:        1;  // Sector Lockdown Enabled
            uint8_t :           1;  // reserved
            uint8_t EPE:        1;  // Erase/Program Error
            uint8_t :           1;  // reserved
            uint8_t READY2:     1;
            uint8_t PG_SIZE:    1;  // 1 if "power of 2" binary page size (512 bytes) is used,
                                    // 0 if standard DataFlash page size (528 bytes)
            uint8_t PROTECT:    1;  // 1 if sector protection is enabled
            uint8_t DENSITY:    4;  // 16-Mbit should be 1011
            uint8_t COMP:       1;  // 1 if main memory page data does not match buffer data
            uint8_t READY:      1;  // RDY/BUSY: 1 = ready, 0 = busy
        } __attribute__((packed));
    } StatusRegister;

    /// Flash device info, consists of 8-bit manufacturerId and 16-bit deviceId.
    typedef struct {
        uint8_t manufacturerId;
        union {
            uint16_t deviceId;
            struct {
                uint8_t deviceIdL;
                uint8_t deviceIdH;
            };
        };
    } DeviceInfo;

    typedef union {
        struct {
            uint8_t byte0;
            uint8_t byte1;
            uint8_t byte2;
        };
        struct {
            uint16_t startAddr: 9;
            uint16_t pageAddr: 12;
        } __attribute__((packed));
    } PageAddress;

    /// Partition Table Entry for FAT file system
    typedef union {
        // uint16_t PTE;
        uint8_t bytes[16];
        struct {
            uint32_t sectors;           // number of sectors
            uint32_t LBA;               // Logical Block Address of first absolute sector in partition

            // CHS address of last absolute sector in partition
            uint8_t  chs_last;
            uint8_t  chs_last_sector;
            uint8_t  chs_last_head;

            uint8_t  type;              // partition type, 0x01 = FAT12

            // CHS address of first absolute sector in partition
            uint8_t  chs_first;
            uint8_t  chs_first_sector;
            uint8_t  chs_first_head;

            uint8_t  status;            // 0x80 = active/bootable, 0x00 = inactive, 0x01-0x7F = invalid
        };
    } PTE;

    /// Master Boot Record
    typedef union {
        uint8_t bytes[512];
        struct {
            uint16_t boot_signature;            // boot signature, should be 0x55AA     16 bytes
            PTE      pte[4];                    // 4 PTE for primary partitions         64 bytes
            uint8_t  code[446];                 // boostrap/executable code             446 bytes
        };
    } MBR;

    typedef union {
        uint8_t bytes[512];
        struct {
            uint16_t boot_signature;            // boot signature (0x55AA)
            uint8_t  boot_strap[480];           // bootstrap code
            uint16_t hidden_sectors;            // number of hidden sectors
            uint16_t heads;                     // number of heads, 2 if double sided disk
            uint16_t sectors_per_track;         // number of sectors per track (12)
            uint16_t secters_per_FAT;           // number of sectors per FAT (9)
            uint8_t  media_descriptor_type;     // Media descriptor type
            uint16_t sectors_in_fs;             // number of sectors in FileSystem
            uint16_t root_directory_entries;    // number of root directory entries
            uint8_t  copies;                    // number of FAT copies
            uint16_t reserved_sectors;          // number of reserved sectors
            uint8_t  sectors_per_cluster;       // number of sectors per cluster, must be power of 2 (1-128)
            uint16_t bytes_per_sector;          // number of bytes per sector
            uint8_t  version[8];                // OEM name/version
            uint16_t jump;                      // jump to bootstrap
        };
    } FAT12;

    static AdestoFlash& sharedInstance();

    /**
     * Retieves the device's 8-bit manufacturerId and 16-bit deviceId
     * @return
     */
    DeviceInfo getDeviceInfo();

    /**
     * Retrieves the status register of the device.
     * @return Returns the 16-bit status regisster.
     */
    StatusRegister getStatus();

    /**
     * Read a specified page in main memory
     * @param  addr PageAddress consisting of the page address and starting offset to read from.
     * @return      Returns an array of bytes.
     */
    uint8_t* read(PageAddress addr);

    /**
     * Retreives the Master Boot Record at page 0.
     * @return Returns the MBR
     */
    MBR readMasterBootRecord();

    /**
     * Retrieves the info of the FAT12 boot sector
     * @param  addr Address of the boot sector.
     * @return      returns FAT12 boot sector
     */
    FAT12 readBootSector(PageAddress addr);

    void write(PageAddress addr, uint8_t *data, uint32_t length);

    /**
     * Erase a page in memeory.
     * If the EPE status flag is raised, then an error has occured and false is returned.
     *
     * @param  addr 12-bit Page address
     * @return      Returns true if the page is erased successfully.
     */
    bool erase(uint16_t addr);

    /**
     * Erases all unprotected or unlocked down sectors.
     */
    void chipErase();

};

#endif /* ADESTOFLASH_HPP_ */

#include "mint_storage.h"

static MintStorage* storage_instance = nullptr;

// Static member initialization
uint8_t MintStorage::msc_disk[DISK_BLOCK_COUNT][DISK_BLOCK_SIZE] = {
    // First block contains FAT filesystem
    {
        0xEB, 0x3C, 0x90, // Jump instruction
        'M', 'S', 'D', '0', 'S', '5', '.', '0', // MSDOS5.0
        0x00, 0x02,                             // Bytes per sector = 512
        0x01,                                   // Sectors per cluster = 1
        0x01, 0x00,                             // Reserved sectors = 1
        0x01,                                   // Num of FATs = 1
        0x10, 0x00,                             // Max root directory entries = 16
        DISK_BLOCK_COUNT, 0x00,                 // Num of sectors = 16
        0xF8,                                   // Media descriptor = fixed disk
        0x01, 0x00,                             // Sectors per FAT = 1
        0x01, 0x00,                             // Sectors per track = 1
        0x01, 0x00,                             // Num of heads = 1
        0x00, 0x00, 0x00, 0x00,                // Hidden sectors = 0
        0x00, 0x00, 0x00, 0x00,                // Total sectors = 0
        0x80,                                   // Drive number = 0x80
        0x00,                                   // Reserved = 0
        0x29,                                   // Extended boot signature = 0x29
        0x00, 0x00, 0x00, 0x00,                // Volume serial number
        'M', 'I', 'N', 'T', ' ', 'D', 'E', 'V', 'I', 'C', 'E', // Volume label
        'F', 'A', 'T', '1', '2', ' ', ' ', ' ' // Filesystem type
    }
};

MintStorage::MintStorage() : last_write_time(0), disk_changed(false) {
    storage_instance = this;
}

void MintStorage::begin() {
    // Initialize MSC
    usb_msc.setID("Mint", "Bearer Device", "1.0");
    usb_msc.setCapacity(DISK_BLOCK_COUNT, DISK_BLOCK_SIZE);
    usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, nullptr);
    usb_msc.setUnitReady(true);
    usb_msc.begin();

    // Create initial README
    const char* readme = "MINT DEVICE\r\nDrop file for wallet\r\n";
    writeFile(readme);
}

void MintStorage::clearDisk() {
    // Clear all blocks except boot sector
    for(int i = 1; i < DISK_BLOCK_COUNT; i++) {
        memset(msc_disk[i], 0, DISK_BLOCK_SIZE);
    }
    disk_changed = true;
}

void MintStorage::writeFile(const char* content) {
    // Write to block 1 (after boot sector)
    size_t len = strlen(content);
    memcpy(msc_disk[1], content, min(len, (size_t)DISK_BLOCK_SIZE));
    disk_changed = true;
}

uint8_t* MintStorage::getFileData() {
    // Return block 3 where uploaded file data typically lands
    return (uint8_t*)&msc_disk[3][0];
}

bool MintStorage::checkNewFile() {
    if (disk_changed && (millis() - last_write_time > 1000)) {
        disk_changed = false;
        return true;
    }
    return false;
}

// Static callbacks
int32_t MintStorage::msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
    memcpy(buffer, msc_disk[lba], bufsize);
    return bufsize;
}

int32_t MintStorage::msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
    memcpy(msc_disk[lba], buffer, bufsize);
    if (storage_instance) {
        storage_instance->last_write_time = millis();
        storage_instance->disk_changed = true;
    }
    return bufsize;
}
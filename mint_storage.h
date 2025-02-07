#ifndef MINT_STORAGE_H
#define MINT_STORAGE_H

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

class MintStorage {
public:
    MintStorage();
    void begin();
    bool checkNewFile();
    void clearDisk();
    void writeFile(const char* content);
    uint8_t* getFileData();
    
private:
    static const uint16_t DISK_BLOCK_SIZE = 512;
    static const uint16_t DISK_BLOCK_COUNT = 16;
    static uint8_t msc_disk[DISK_BLOCK_COUNT][DISK_BLOCK_SIZE];  // Mock storage
    Adafruit_USBD_MSC usb_msc;
    unsigned long last_write_time;
    bool disk_changed;

    void formatDisk();
    static int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize);
    static int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize);
};

#endif // MINT_STORAGE_H

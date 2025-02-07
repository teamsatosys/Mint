// mint.h
#ifndef MINT_H
#define MINT_H

#include <Arduino.h>
#include "mint_storage.h"
#include "mint_led.h"
#include "mint_wallet.h"

// Global definitions
#define CIRCUIT_PIN 14

class MintDevice {
public:
    MintDevice();
    void begin();
    void loop();

private:
    MintStorage storage;
    MintLED led;
    MintWallet wallet;
    bool processing_file;
};

#endif
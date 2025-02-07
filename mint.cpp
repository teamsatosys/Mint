// mint.cpp
#include "mint.h"

MintDevice::MintDevice() : processing_file(false) {}

void MintDevice::begin() {
    Serial.begin(115200);
    delay(2000);  // Give time for Serial to initialize
    Serial.println("\n\nMint device starting...");
    
    pinMode(CIRCUIT_PIN, INPUT_PULLUP);
    
    Serial.println("Initializing LED...");
    led.begin();
    led.setInitializing();  // Should show blue
    
    Serial.println("Initializing storage...");
    storage.begin();
    
    led.setIntact();  // Should show red when circuit is connected
    Serial.println("Mint device ready");
}

void MintDevice::loop() {
    bool circuit_intact = !digitalRead(CIRCUIT_PIN);
    
    // Always update LED based on circuit state unless processing
    if (!processing_file) {
        if (circuit_intact) {
            led.setIntact();
        } else {
            led.setBroken();
        }
    }
    
    if (!processing_file && storage.checkNewFile()) {
        Serial.println("New file detected!");
        processing_file = true;
        led.setGenerating();
        // ... rest of the processing code ...
        processing_file = false;
        Serial.println("File processing complete");
    }
    
    delay(100);  // Add a small delay to prevent tight looping
}
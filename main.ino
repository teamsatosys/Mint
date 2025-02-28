#if defined(ARDUINO_ARCH_RP2040)
#include <Adafruit_TinyUSB.h>
#include <Wire.h>
#endif

#include "mint.h"

MintDevice mint;

void setup() {
    // Initialize serial for debugging
    #ifdef DEBUG_ENABLED
    Serial.begin(115200);
    delay(2000); // Wait for serial connection
    Serial.println("Mint Bitcoin Bearer Device");
    Serial.println("Initializing...");
    #endif
    
    // Initialize the device
    if (!mint.begin()) {
        #ifdef DEBUG_ENABLED
        Serial.println("Initialization failed!");
        #endif
        
        // If initialization fails, flash LED rapidly to indicate error
        while (1) {
            // Error state will be shown by the LED
            delay(100);
        }
    }
    
    #ifdef DEBUG_ENABLED
    Serial.println("Initialization complete");
    Serial.println("Device ready");
    #endif
}

void loop() {
    // Run main device loop
    mint.loop();
    
    // Small delay to prevent CPU hogging
    delay(10);
}
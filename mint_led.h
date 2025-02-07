// mint_led.h
#ifndef MINT_LED_H
#define MINT_LED_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class MintLED {
public:
    MintLED(uint8_t pin = 16);
    void begin();
    void setInitializing();
    void setIntact();
    void setBroken();
    void setGenerating();
    
private:
    static const uint8_t NUM_PIXELS = 1;
    Adafruit_NeoPixel pixels;
    const uint32_t COLOR_INTACT = 0x002000;     // Green for intact
    const uint32_t COLOR_BROKEN = 0x200000;     // Red for broken
    const uint32_t COLOR_INIT = 0x000020;       // Blue
    const uint32_t COLOR_GENERATING = 0x202000; // Yellow
};

#endif

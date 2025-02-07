#include "mint_led.h"

MintLED::MintLED(uint8_t pin) : pixels(NUM_PIXELS, pin, NEO_GRB + NEO_KHZ800) {
}

void MintLED::begin() {
    pixels.begin();
    pixels.setBrightness(50);
    setInitializing();
}

void MintLED::setInitializing() {
    pixels.setPixelColor(0, COLOR_INIT);
    pixels.show();
}

void MintLED::setIntact() {
    pixels.setPixelColor(0, COLOR_INTACT);
    pixels.show();
}

void MintLED::setBroken() {
    pixels.setPixelColor(0, COLOR_BROKEN);
    pixels.show();
}

void MintLED::setGenerating() {
    pixels.setPixelColor(0, COLOR_GENERATING);
    pixels.show();
}
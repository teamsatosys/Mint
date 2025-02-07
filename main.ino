#if defined(ARDUINO_ARCH_RP2040)
#include <Adafruit_TinyUSB.h>
#endif

#include "mint.h"

MintDevice mint;

void setup() {
    mint.begin();
}

void loop() {
    mint.loop();
}
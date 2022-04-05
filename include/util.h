#pragma once

#include <Arduino.h>
#include <NeoPixelBus.h>

/* used for throwaway fraction for modf */
extern double _fractInt;

/** returns the fractional part of a floating point number(e.g. 23.12 = > 0.12) */
#define fract(x) modf(x, &_fractInt);

void util_setup();
void util_loop();
void save_eeprom(bool defer = true);

#define EEPROM_MAGIC 0x45

struct EEPROMContent {
    uint8_t magic;
    char app[32];
    float brightness;
    RgbColor primaryColor;
    uint16_t currentLimit;
};

extern EEPROMContent eepromContent;
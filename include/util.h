#pragma once

#include <Arduino.h>

/* used for throwaway fraction for modf */
extern double _fractInt;

/** returns the fractional part of a floating point number(e.g. 23.12 = > 0.12) */
#define fract(x) modf(x, &_fractInt);

void util_setup();
void save_eeprom();

#define EEPROM_MAGIC 0x42

struct EEPROMContent {
    uint8_t magic;
    char app[32];
    float brightness;
};

extern EEPROMContent eepromContent;
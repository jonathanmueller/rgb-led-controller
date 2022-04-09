

#pragma once

#include "led.h"
#include <Arduino.h>

#define MAX_LED_GROUPS 5

void eeprom_setup();
void eeprom_loop();
void save_eeprom(bool defer = true);

#define EEPROM_MAGIC 0x47

struct EEPROMLightState {
    RgbColor color;
    float brightness;
    bool isOn;
};

struct EEPROMContent {
    uint8_t magic;
    char app[32];
    char notificationServer[64];
    uint16_t currentLimit;
    EEPROMLightState primaryLightState;
    EEPROMLightState ledGroupStates[MAX_LED_GROUPS];
};

extern EEPROMContent eepromContent;


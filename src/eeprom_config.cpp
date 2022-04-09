#include "eeprom_config.h"
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SAVE_DELAY 5000

/* content saved in eeprom */
EEPROMContent eepromContent;

void eeprom_setup() {

    /* Load configuration from EEPROM */
    EEPROM.begin(sizeof(EEPROMContent));
    EEPROM.get(0, eepromContent);

    if (eepromContent.magic != EEPROM_MAGIC) {
        Serial.println("No configuration found. Loading default.");
        eepromContent = EEPROMContent{
            .magic = EEPROM_MAGIC,
            .app = {'c','y','c','l','e',0},
            .notificationServer = { 0 },
            .currentLimit = 1000,
            .primaryLightState = {
                .color = RgbColor(255,128,0),
                .brightness = 1.0f,
                .isOn = true
            },
            .ledGroupStates = {
                { .color = RgbColor(255,0,0), .brightness = 1.0f, .isOn = true },
                { .color = RgbColor(0,0,255), .brightness = 1.0f, .isOn = true },
                { .color = RgbColor(0,255,0), .brightness = 1.0f, .isOn = true },
                { .color = RgbColor(255,255,0), .brightness = 1.0f, .isOn = true },
                { .color = RgbColor(0,255,255), .brightness = 1.0f, .isOn = true }
            }
        };
        save_eeprom(false);
    }
}

unsigned long eeprom_save_requested = 0;


void save_eeprom(bool defer) {
    if (defer) {
        eeprom_save_requested = millis();
        return;
    }

    Serial.println("Saving to EEPROM...");

    eepromContent.magic = EEPROM_MAGIC;
    EEPROM.put(0, eepromContent);
    EEPROM.commit();
}

void eeprom_loop() {
    /* Do deferred EEPROM save */
    if (eeprom_save_requested && (millis() >= eeprom_save_requested + EEPROM_SAVE_DELAY)) {
        eeprom_save_requested = 0;
        save_eeprom(false);
    }
}
#include "util.h"
#include "EEPROM.h"

#include <Arduino.h>

/* used for throwaway fraction for modf */
double _fractInt;

/* content saved in eeprom */
EEPROMContent eepromContent;

void SetRandomSeed() {
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3) {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

void util_setup() {
    SetRandomSeed();

    /* Load configuration from EEPROM */
    EEPROM.begin(sizeof(EEPROMContent));
    EEPROM.get(0, eepromContent);

    if (eepromContent.magic != EEPROM_MAGIC) {
        Serial.println("No configuration found. Loading default.");
        eepromContent = EEPROMContent{
            .magic = EEPROM_MAGIC,
            .app = {'c','y','c','l','e',0},
            .brightness = 1.0f,
            .primaryColor = RgbColor(255,128,0),
            .currentLimit = 1000
        };
        save_eeprom();
    }
}

unsigned long eeprom_save_requested = 0;

#define EEPROM_SAVE_DELAY 5000

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

void util_loop() {
    /* Do deferred EEPROM save */
    if (eeprom_save_requested && (millis() >= eeprom_save_requested + EEPROM_SAVE_DELAY)) {
        eeprom_save_requested = 0;
        save_eeprom(false);
    }
}
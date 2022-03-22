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
            .brightness = 1.0f
        };
        save_eeprom();
    }
}

void save_eeprom() {
    eepromContent.magic = EEPROM_MAGIC;
    EEPROM.put(0, eepromContent);
    EEPROM.commit();
}
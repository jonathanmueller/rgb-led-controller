#pragma once

typedef enum {
    Characteristic_IsOn = 1 << 0,
    Characteristic_Brightness = 1 << 1,
    Characteristic_Color = 1 << 2,

    Characteristic_All = 0xFF
} Characteristic;

void notify_status_change(Characteristic changedCharacteristics = Characteristic_All);
void api_setup();
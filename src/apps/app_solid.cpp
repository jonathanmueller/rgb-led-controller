#include "led.h"
#include <Arduino.h>

REGISTER_APP(solid) {
    void setup() {
        appRespectsPrimaryColor = true;
    }

    void loop() {
        strip.ClearTo(primaryColor);
        strip.Show();
    }
}
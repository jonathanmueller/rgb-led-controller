#include "led.h"
#include <Arduino.h>

REGISTER_APP(solidcolors) {
    RgbColor color;
    void setup() {
        color = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
        markAppCycle();
    }

    unsigned long lastChange = 0;
    void loop() {
        if (millis() > lastChange + 1000) {
            lastChange = millis();
            setup();
            markAppCycle();
        }

        strip.ClearTo(color);
        strip.Show();
    }
}
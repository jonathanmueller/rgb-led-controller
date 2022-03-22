#include "led.h"
#include <Arduino.h>

REGISTER_APP(solid) {
    RgbColor color;
    void setup() {
        color = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
        markAppCycle();
    }

    long lastChange = 0;
    void loop() {
        if (millis() > lastChange + 500) {
            lastChange = millis();
            setup();
            markAppCycle();
        }

        fill(color);
        strip.Show();
    }
}
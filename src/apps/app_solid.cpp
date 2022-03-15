#include "led.h"
#include <Arduino.h>

REGISTER_APP(solid) {
    void setup() {

    }

    void loop() {
        fill(HslColor(random(360) / 360.0f, 1.0f, 0.25f));
        strip.Show();
        delay(500);
    }
}
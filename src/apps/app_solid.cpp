#include "led.h"
#include <Arduino.h>

REGISTER_APP(solid) {
    void setup() {
        appRespectsPrimaryColor = true;
    }

    void loop() {
        fill(primaryColor);
        strip.Show();
    }
}
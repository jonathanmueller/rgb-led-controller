#include "led.h"

REGISTER_APP(strobe) {
    void setup() {
        appRespectsPrimaryColor = true;
    }

    void loop() {
        strip.ClearTo(primaryColor);
        strip.Show();
        delay(30);
        strip.ClearTo(0);
        strip.Show();
        delay(30);
    }
}

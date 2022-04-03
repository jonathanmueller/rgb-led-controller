#include "led.h"

REGISTER_APP(strobe) {
    void setup() {
        appRespectsPrimaryColor = true;
    }

    void loop() {
        fill(primaryColor);
        strip.Show();
        delay(100);
        fill(0);
        strip.Show();
        delay(100);
    }
}

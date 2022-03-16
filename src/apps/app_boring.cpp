#include "led.h"

REGISTER_APP(boring) {
    void setup() {
        fill(colorGamma.Correct(RgbColor(90, 40, 0)));
        strip.Show();
    }

    void loop() {}
}
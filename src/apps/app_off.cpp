#include "led.h"

REGISTER_APP(off) {
    void setup() {}

    void loop() {
        fill(0);
        strip.Show();
    }
}

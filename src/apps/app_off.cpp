#include "led.h"

REGISTER_APP(off) {
    void setup() {}

    void loop() {
        strip.ClearTo(0);
        strip.Show();
    }
}

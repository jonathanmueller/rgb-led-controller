#include "led.h"
#include "shader.h"
#include <Arduino.h>

REGISTER_APP(stroke) {
    int index;
    RgbColor color;

    void setup() {
        index = 0;
        color = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
    }

    long waitUntil = 0;
    void loop() {
        if (waitUntil > millis()) {
            return;
        }

        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(PixelStrokeOrder[i], i <= index ? color : RgbColor(0));
        }
        strip.Show();

        index++;
        if (index >= PixelCount) {
            waitUntil = millis() + 2000; /* This causes a delay without blocking everything */
            setup(); /* reset */
        }
        delay(20);
    }
}
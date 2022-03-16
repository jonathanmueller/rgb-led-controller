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

    void loop() {
        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(PixelStrokeOrder[i], i <= index ? color : RgbColor(0));
        }
        strip.Show();

        index++;
        if (index >= PixelCount) {
            delay(2000);
            setup(); /* reset */
        }
        delay(50);
    }
}
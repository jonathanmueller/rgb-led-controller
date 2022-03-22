#include "led.h"
#include "shader.h"
#include <Arduino.h>

REGISTER_APP(letters) {
    int letterIndex;
    RgbColor color;

    void setup() {
        letterIndex = 0;
        color = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
        markAppCycle();
    }

    void loop() {
        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, PixelLetterNumbers[i] == letterIndex ? color : RgbColor(0));
        }
        letterIndex++;
        if (letterIndex >= NumberOfLetters) {
            setup(); /* reset */
            markAppCycle();
        }
        strip.Show();
        delay(500);
    }
}
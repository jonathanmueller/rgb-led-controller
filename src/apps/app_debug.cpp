#include "led.h"

REGISTER_APP(debug) {
    int index = 0;

    int colorIndex = 0;
    std::vector<RgbColor> colors = { RgbColor(255,0,0), RgbColor(0,255,0), RgbColor(0,0,255), RgbColor(255) };

    void setup() {
    }


    void loop() {
        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, i == index ? colors[colorIndex] : RgbColor(0));
        }
        index++;
        if (index >= PixelCount) {
            index = 0;
            colorIndex = (colorIndex + 1) % colors.size();
        }
        strip.Show();
        delay(100);
    }
}
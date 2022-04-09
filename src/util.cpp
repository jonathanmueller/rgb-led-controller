#include "util.h"

#include <Arduino.h>

/* used for throwaway fraction for modf */
double _fractInt;

void SetRandomSeed() {
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3) {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

void util_setup() {
    SetRandomSeed();
}

// Operators

String toHexString(const RgbColor &color) {
    char hexColor[9];
    HtmlColor(color).ToNumericalString(hexColor, 9);
    return "\"" + String(hexColor + 1) + "\"";
}

RgbColor operator *(const RgbColor &color, float factor) {
    return RgbColor(color.R * factor, color.G * factor, color.B * factor);
}

RgbColor operator +(const RgbColor &color1, const RgbColor &color2) {
    RgbColor result = color1;
    if (result.R < RgbColor::Max - color2.R) { result.R += color2.R; } else { result.R = RgbColor::Max; }
    if (result.G < RgbColor::Max - color2.G) { result.G += color2.G; } else { result.G = RgbColor::Max; }
    if (result.B < RgbColor::Max - color2.B) { result.B += color2.B; } else { result.B = RgbColor::Max; }

    return result;
}

void operator +=(RgbColor &color, const RgbColor &operand) {
    color = color + operand;
}

#pragma once

#include <NeoPixelBus.h>

/* used for throwaway fraction for modf */
extern double _fractInt;

/** returns the fractional part of a floating point number(e.g. 23.12 = > 0.12) */
#define fract(x) modf(x, &_fractInt);

void util_setup();

String toHexString(const RgbColor &color);
RgbColor operator *(const RgbColor &color, float factor);
RgbColor operator +(const RgbColor &color1, const RgbColor &color2);
void operator +=(RgbColor &color, const RgbColor &operand);

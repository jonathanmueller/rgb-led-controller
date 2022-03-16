#pragma once

/* used for throwaway fraction for modf */
extern double _fractInt;

/** returns the fractional part of a floating point number(e.g. 23.12 = > 0.12) */
#define fract(x) modf(x, &_fractInt);

void util_setup();
#include "led.h"
#include <Arduino.h>

/* The number of connected LEDs */

/* The physical coordinates of each LED, used for some effects */
const std::array<vec2<float>, PixelCount> PixelPositions{ {
    {0.7330f, 1.0070f}, {0.6000f, 0.9850f}, {0.4770f, 0.9880f}, {0.3460f, 0.9830f}, {0.2230f, 0.9720f}, {0.0920f, 0.9610f}, {-0.0310f, 0.9400f}, {-0.1560f, 0.9190f}, {-0.2740f, 0.8950f}, {-0.4020f, 0.8810f}, {0.1940f, 0.8300f}, {0.1960f, 0.7080f}, {0.1940f, 0.5820f}, {0.1990f, 0.4540f}, {0.1990f, 0.3280f}, {0.1860f, 0.1810f}, {0.1530f, 0.0560f}, {0.1190f, 0.0590f}, {0.0870f, 0.1930f}, {0.0570f, 0.3080f}, {0.0250f, 0.4330f}, {-0.0100f, 0.5640f}, {-0.0500f, 0.6820f}, {-0.0790f, 0.8100f}, {-0.1110f, 0.9360f}, {-0.1480f, 1.0590f}, {-0.2180f, 1.1840f}, {-0.3030f, 1.2910f}, {-0.3830f, 1.3900f}, {-0.4610f, 1.4910f}, {-0.5620f, 1.5720f}, {-0.6930f, 1.6060f}, {-0.8110f, 1.5610f}, {-0.8560f, 1.4300f}, {-0.8460f, 1.2990f}, {-0.8030f, 1.1650f}, {-0.7440f, 1.0480f}, {-0.6850f, 0.9440f}, {-0.6290f, 0.8260f}, {-0.5410f, 0.7270f}, {-0.4420f, 0.6390f}, {-0.3490f, 0.5560f}, {-0.2500f, 0.4730f}, {-0.1480f, 0.3880f}, {-0.0520f, 0.3160f}
} };

const std::array<int, PixelCount> PixelStrokeOrder{ {
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
} };

/* The index of the letter which each LED belongs to */
const std::array<int, PixelCount> PixelLetterNumbers{ {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
} };



////////////////// Derived values //////////////////

const Corners CanvasCorners = [] {
    float minX = INFINITY;
    float maxX = -INFINITY;
    float minY = INFINITY;
    float maxY = -INFINITY;
    for (int i = 0; i < PixelCount; i++) {
        if (PixelPositions[i].x < minX) { minX = PixelPositions[i].x; }
        if (PixelPositions[i].y < minY) { minY = PixelPositions[i].y; }
        if (PixelPositions[i].x > maxX) { maxX = PixelPositions[i].x; }
        if (PixelPositions[i].y > maxY) { maxY = PixelPositions[i].y; }
    }
    return Corners{ .MinX = minX, .MaxX = maxX, .MinY = minY, .MaxY = maxY };
}();

const vec2<float> CanvasSize = [] { return vec2<float>{ CanvasCorners.MaxX - CanvasCorners.MinX, CanvasCorners.MaxY - CanvasCorners.MinY }; }();

const int NumberOfLetters = []() {
    int maxLetterNumber = -1;
    for (int i = 0; i < PixelCount; i++) {
        if (PixelLetterNumbers[i] > maxLetterNumber) { maxLetterNumber = PixelLetterNumbers[i]; }
    }
    return maxLetterNumber + 1;
}();


const std::array<vec2<float>, PixelCount> NormalizedPixelPositions = []() {
    std::array<vec2<float>, PixelCount> result;
    vec2<float> minCorner{ CanvasCorners.MinX, CanvasCorners.MinY };
    for (int i = 0; i < PixelCount; i++) {
        result[i] = PixelPositions[i] - minCorner;
        result[i].x = ((result[i].x / CanvasSize.x) * 2.0f) - 1.0f;
        result[i].y = ((result[i].y / CanvasSize.y) * 2.0f) - 1.0f;
    }
    return result;
}();

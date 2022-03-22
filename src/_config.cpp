#include "led.h"
#include <Arduino.h>

/* The number of connected LEDs */

/* The physical coordinates of each LED, used for some effects */
const std::array<vec2<float>, PixelCount> PixelPositions{ {
    {567.0f, 108.0f}, {522.0f, 104.0f}, {479.0f, 101.0f}, {436.0f, 98.0f}, {399.0f, 96.0f}, {367.0f, 98.0f}, {329.0f, 100.0f}, {294.0f, 107.0f}, {259.0f, 111.0f}, {217.0f, 115.0f}, {400.0f, 148.0f}, {394.0f, 188.0f}, {394.0f, 223.0f}, {387.0f, 264.0f}, {386.0f, 296.0f}, {386.0f, 337.0f}, {377.0f, 375.0f}, {368.0f, 411.0f}, {361.0f, 446.0f}, {349.0f, 479.0f}, {334.0f, 515.0f}, {326.0f, 546.0f}, {320.0f, 584.0f}, {306.0f, 610.0f}, {294.0f, 646.0f}, {281.0f, 673.0f}, {264.0f, 706.0f}, {241.0f, 743.0f}, {211.0f, 767.0f}, {187.0f, 794.0f}, {151.0f, 814.0f}, {121.0f, 818.0f}, {84.0f, 790.0f}, {70.0f, 758.0f}, {79.0f, 716.0f}, {102.0f, 675.0f}, {117.0f, 646.0f}, {140.0f, 622.0f}, {153.0f, 598.0f}, {183.0f, 575.0f}, {212.0f, 551.0f}, {242.0f, 530.0f}, {267.0f, 503.0f}, {294.0f, 488.0f}, {322.0f, 467.0f}, {400.0f, 425.0f}, {431.0f, 408.0f}, {467.0f, 395.0f}, {506.0f, 384.0f}, {539.0f, 370.0f}, {592.0f, 460.0f}, {617.0f, 427.0f}, {641.0f, 392.0f}, {650.0f, 358.0f}, {639.0f, 324.0f}, {596.0f, 339.0f}, {577.0f, 369.0f}, {559.0f, 406.0f}, {554.0f, 439.0f}, {550.0f, 474.0f}, {556.0f, 513.0f}, {571.0f, 544.0f}, {615.0f, 548.0f}, {659.0f, 515.0f}, {678.0f, 488.0f}, {698.0f, 455.0f}, {727.0f, 423.0f}, {805.0f, 46.0f}, {792.0f, 88.0f}, {787.0f, 130.0f}, {779.0f, 169.0f}, {773.0f, 208.0f}, {768.0f, 245.0f}, {762.0f, 282.0f}, {763.0f, 317.0f}, {758.0f, 355.0f}, {757.0f, 395.0f}, {755.0f, 433.0f}, {750.0f, 470.0f}, {747.0f, 505.0f}, {749.0f, 535.0f}, {747.0f, 569.0f}, {746.0f, 603.0f}, {744.0f, 647.0f}, {800.0f, 358.0f}, {815.0f, 324.0f}, {834.0f, 296.0f}, {854.0f, 267.0f}, {874.0f, 235.0f}, {894.0f, 206.0f}, {920.0f, 172.0f}, {792.0f, 449.0f}, {826.0f, 440.0f}, {861.0f, 435.0f}, {888.0f, 423.0f}, {927.0f, 403.0f}, {961.0f, 382.0f},
} };

const std::array<int, PixelCount> PixelStrokeOrder{ {
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    57,
    50, 51, 52, 53, 54, 55, 56,
    58, 59, 60, 61, 62, 63, 64, 65, 66,
    67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
    84, 85, 86, 87, 88, 89, 90,
    91, 92, 93, 94, 95, 96,
    9, 8, 7, 6, 5, 4, 3, 2, 1, 0
} };

/* The index of the letter which each LED belongs to */
const std::array<int, PixelCount> PixelLetterNumbers{ {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
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

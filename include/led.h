#pragma once

#include "app.h"
#include "corners.h"
#include "vec2.h"
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

const uint16_t PixelCount = 45;

/* The physical coordinates of each LED, used for some effects */
extern const std::array<vec2<float>, PixelCount> PixelPositions;

extern const std::array<int, PixelCount> PixelStrokeOrder;

/* The index of the letter which each LED belongs to */
extern const std::array<int, PixelCount> PixelLetterNumbers;

extern const int NumberOfLetters;
extern const Corners CanvasCorners;
extern const vec2<float> CanvasSize;
extern const std::array<vec2<float>, PixelCount> NormalizedPixelPositions;


extern NeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip;
extern NeoGamma<NeoGammaTableMethod> colorGamma;

void led_setup();
void led_loop();

const String& getApp();
bool setApp(const String& name);

void fill(const RgbColor& color);
#pragma once

#include "app.h"
#include "vec2.h"
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

const uint16_t PixelCount = 45;
extern NeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip;
extern NeoGamma<NeoGammaTableMethod> colorGamma;
extern std::array<vec2<float>, PixelCount> pixelPositions;
extern std::array<int, PixelCount> letterNumbers;

void led_setup();
void led_loop();

const String& getApp();
bool setApp(const String& name);

void fill(const RgbColor& color);
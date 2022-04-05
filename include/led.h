#pragma once

#include "app.h"
#include "corners.h"
#include "vec2.h"
#include <Arduino.h>
#include "CustomNeoPixelBus.h"
#include "NeoPixelAnimator.h"

#define ON_OFF_FADE_TIME 500
#define MAX_BRIGHTNESS 1.0f

const uint16_t PixelCount = 97;

/* The physical coordinates of each LED, used for some effects */
extern const std::array<vec2<float>, PixelCount> PixelPositions;

extern const std::array<int, PixelCount> PixelStrokeOrder;

/* The index of the letter which each LED belongs to */
extern const std::array<int, PixelCount> PixelLetterNumbers;

extern const int NumberOfLetters;
extern const Corners CanvasCorners;
extern const vec2<float> CanvasSize;
extern const std::array<vec2<float>, PixelCount> NormalizedPixelPositions;


extern CustomNeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip;
extern NeoGamma<NeoGammaTableMethod> colorGamma;

bool isOn();
void setOn(bool on, bool fade = true);
extern RgbColor primaryColor;
extern bool appRespectsPrimaryColor;

void led_setup();
void led_loop();

float getFPS();

const String& getApp();
bool setApp(const String& name, bool save = true);

void setBrightness(float brightness, bool save = true);
float getBrightness();

void setCurrentLimit(uint16_t currentLimit, bool save = true);
uint16_t getCurrentLimit();

void setPrimaryColor(const RgbColor& primaryColor, bool makeVisible = false, bool save = true);

void fill(const RgbColor& color);
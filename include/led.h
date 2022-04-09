#pragma once

#include "app.h"
#include "corners.h"
#include "vec2.h"
#include "util.h"
#include "eeprom_config.h"
#include <Arduino.h>
#include "CustomNeoPixelBus.h"
#include "NeoPixelAnimator.h"
#include "AnimatedValue.h"

#define ON_OFF_FADE_TIME 500
#define BRIGHTNESS_ANIMATION_TIME 500
#define COLOR_ANIMATION_TIME 500
#define MAX_BRIGHTNESS 1.0f


const uint16_t PixelCount = 97;

/* The physical coordinates of each LED, used for some effects */
extern const std::array<vec2<float>, PixelCount> PixelPositions;

/* The order of leds when stroking the strip */
extern const std::array<int, PixelCount> PixelStrokeOrder;

/* The index of the letter which each LED belongs to */
extern const std::array<int, PixelCount> PixelLetterNumbers;

/* Defines an area as a group of LEDs */
struct LEDGroup {
    uint8_t button;
    float minX;
    float maxX;
    float glowX;

    float minY;
    float maxY;
    float glowY;
};

/* The light groups */
extern const std::array<LEDGroup, 2> LEDGroups;

extern const int NumberOfLetters;
extern const Corners CanvasCorners;
extern const vec2<float> CanvasSize;
extern const std::array<vec2<float>, PixelCount> NormalizedPixelPositions;


extern CustomNeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip;

struct FadingLightState {
    bool isOn;
    bool isToggling;
    unsigned int toggleTime;

    void setOn(bool on, unsigned int fadeTime = ON_OFF_FADE_TIME);
    float getBrightnessFactor();
};


bool isOn();
void setOn(bool on, bool fade = true, bool save = true);

void setLEDGroupColor(uint8_t i, const RgbColor &color, bool fade = true, bool save = true);
void setLEDGroupBrightness(uint8_t i, float brightness, bool fade = true, bool save = true);
void setLEDGroupOn(uint8_t i, bool on, bool fade = true, bool save = true);

bool isLEDGroupOn(uint8_t i);
float getLEDGroupBrightness(uint8_t i);
RgbColor getLEDGroupColor(uint8_t i);

extern AnimatedValue<RgbColor> primaryColor;
extern bool appRespectsPrimaryColor;

void led_setup();
void led_loop();

float getFPS();

const App& getApp();
bool setApp(const String& name, bool save = true);

void setBrightness(float brightness, bool fade = true, bool save = true);
float getBrightness();

void setCurrentLimit(uint16_t currentLimit, bool save = true);
uint16_t getCurrentLimit();

void setPrimaryColor(const RgbColor& primaryColor, bool makeVisible = true, bool fade = true, bool save = true);
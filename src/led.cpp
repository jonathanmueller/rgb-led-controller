#include "led.h"
#include "app.h"
#include "api.h"
#include "util.h"
#include "corners.h"
#include <tuple>

NeoGamma<NeoGammaTableMethod> colorGamma;
CustomNeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip(PixelCount, NeoRgbCurrentSettings(160, 160, 160));

float _brightness = 1.0f;
uint16_t _currentLimit = 1000;
bool _isOn = true;
RgbColor primaryColor;
bool appRespectsPrimaryColor = false;
String currentAppName;
App currentApp;
float fps;
unsigned long lastFrame = 0;

bool isToggling = false;
unsigned long toggleTime = 0;
void setOn(bool on, bool fade) {
    if (fade) {
        if (on != _isOn) {
            if (!isToggling) {
                toggleTime = millis();
            } else {
                toggleTime = 2 * millis() - ON_OFF_FADE_TIME - toggleTime;
            }
            isToggling = true;
        }
    } else {
        isToggling = false;
        toggleTime = 0;
    }

    _isOn = on;

    notify_status_change(Characteristic_IsOn);
}

bool isOn() {
    return _isOn;
}

void led_setup() {
    strip.Begin();

    Serial.printf("Loading app %s\n", eepromContent.app);
    if (!setApp(eepromContent.app, false)) {
        setApp("cycle");
    }

    setBrightness(eepromContent.brightness, false);
    setCurrentLimit(eepromContent.currentLimit, false);
    setPrimaryColor(eepromContent.primaryColor, false, false);
}

void led_loop() {
    /* Calculate brightness */
    float brightness = _brightness * MAX_BRIGHTNESS;

    if (isToggling) {
        /* Fade in or out after toggling */
        unsigned long timeSinceToggle = millis() - toggleTime;
        if (timeSinceToggle >= ON_OFF_FADE_TIME) {
            isToggling = false;
        }

        uint8_t progress = 255.0f * min(max((float)timeSinceToggle / ON_OFF_FADE_TIME, 0.0f), 1.0f);
        float fadeFactor = NeoGammaEquationMethod::Correct(_isOn ? progress : 255 - progress) / 255.0f;

        brightness *= fadeFactor;
    } else if (!_isOn) {
        brightness = 0;
    }

    strip.SetBrightness(brightness);
    strip.Show();

    std::get<1>(currentApp)();

    unsigned long currentTime = millis();
    unsigned long delta = max(1UL, currentTime - lastFrame);
    float currentFPS = 1000.0f / delta;
    fps = 0.2 * fps + 0.8 * currentFPS;
    lastFrame = currentTime;
}

const String& getApp() {
    return currentAppName;
}

bool setApp(const String& name, bool save) {
    if (getApps().find(name) == getApps().end()) {
        return false;
    }

    currentApp = getApps().at(name);
    currentAppName = name;
    appRespectsPrimaryColor = false;

    std::get<0>(currentApp)();

    if (save) {
        strncpy(eepromContent.app, currentAppName.c_str(), sizeof(((EEPROMContent*)0)->app));
        save_eeprom();
    }

    return true;
}



void setBrightness(float brightness, bool save) {
    _brightness = brightness;
    strip.SetBrightness(brightness * MAX_BRIGHTNESS);
    if (save) {
        eepromContent.brightness = brightness;
        save_eeprom();
    }
    notify_status_change(Characteristic_Brightness);
}

float getBrightness() {
    return _brightness;
}


void setCurrentLimit(uint16_t currentLimit, bool save) {
    _currentLimit = currentLimit;
    strip.SetCurrentLimit(currentLimit);
    if (save) {
        eepromContent.currentLimit = currentLimit;
        save_eeprom();
    }
}

uint16_t getCurrentLimit() {
    return _currentLimit;
}

float getFPS() {
    return fps;
}

void setPrimaryColor(const RgbColor& color, bool makeVisible, bool save) {
    primaryColor = color;
    if (save) {
        eepromContent.primaryColor = color;
    }

    if (makeVisible && !appRespectsPrimaryColor) {
        setApp("solid", save);
    } else if (save) {
        save_eeprom();
    }

    notify_status_change(Characteristic_Color);
}

void fill(const RgbColor& color) {
    for (int i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, color);
    }
}

#include "led.h"
#include "app.h"
#include "util.h"
#include "corners.h"
#include <tuple>

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip(PixelCount);

uint8_t _brightness = 255;
bool _isOn = true;
RgbColor primaryColor;
bool appRespectsPrimaryColor = false;
String currentAppName;
App currentApp;

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
    setPrimaryColor(eepromContent.primaryColor, false, false);
}

void led_loop() {
    if (!isToggling && !_isOn) {
        fill(0);
        strip.Show();
        return;
    }

    /* Fade in or out after toggling */
    if (isToggling) {
        unsigned long timeSinceToggle = millis() - toggleTime;
        if (timeSinceToggle >= ON_OFF_FADE_TIME) {
            isToggling = false;
        }

        uint8_t progress = 255.0f * min(max((float)timeSinceToggle / ON_OFF_FADE_TIME, 0.0f), 1.0f);
        float factor = NeoGammaEquationMethod::Correct(_isOn ? progress : 255 - progress) / 255.0f;
        strip.SetBrightness(_brightness * factor * MAX_BRIGHTNESS);
    }

    std::get<1>(currentApp)();
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



void setBrightness(uint8_t brightness, bool save) {
    _brightness = brightness;
    strip.SetBrightness(brightness * MAX_BRIGHTNESS);
    if (save) {
        eepromContent.brightness = brightness;
        save_eeprom();
    }
}

uint8_t getBrightness() {
    return _brightness;
}

void setPrimaryColor(const RgbColor &color, bool makeVisible, bool save) {
    primaryColor = color;
    if (save) {
        eepromContent.primaryColor = color;
    }

    if (makeVisible && !appRespectsPrimaryColor) {
        setApp("solid", save);
    } else if (save) {
        save_eeprom();
    }
}

void fill(const RgbColor& color) {
    for (int i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, color);
    }
}

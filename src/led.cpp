#include "led.h"
#include "app.h"
#include "api.h"
#include "util.h"
#include "corners.h"
#include <tuple>

CustomNeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip(PixelCount, NeoRgbCurrentSettings(160, 160, 160));

uint16_t _currentLimit = 1000;
AnimatedValue<RgbColor> primaryColor(0);
AnimatedValue<float> _brightness = 1.0f;
bool appRespectsPrimaryColor = false;
App currentApp;
float fps;
unsigned long lastFrame = 0;

FadingLightState lightState = {
    .isOn = true,
    .isToggling = false,
    .toggleTime = 0
};

void FadingLightState::setOn(bool on, unsigned int fadeTime) {
    if (fadeTime > 0) {
        if (on != isOn) {
            if (!isToggling) {
                toggleTime = millis();
            } else {
                toggleTime = 2 * millis() - fadeTime - toggleTime;
            }
            isToggling = true;
        }
    }
    isOn = on;
}

float FadingLightState::getBrightnessFactor() {
    if (isToggling) {
        /* Fade in or out after toggling */
        unsigned long timeSinceToggle = millis() - toggleTime;
        if (timeSinceToggle >= ON_OFF_FADE_TIME) {
            isToggling = false;
        }

        uint8_t progress = 255.0f * min(max((float)timeSinceToggle / ON_OFF_FADE_TIME, 0.0f), 1.0f);
        float fadeFactor = NeoGammaEquationMethod::Correct(isOn ? progress : 255 - progress) / 255.0f;

        return fadeFactor;
    } else if (!isOn) {
        return 0;
    }

    return 1;
}

void setOn(bool on, bool fade, bool save) {
    lightState.setOn(on, fade ? ON_OFF_FADE_TIME : 0);
    notify_status_change(Characteristic_IsOn);

    if (save) {
        eepromContent.primaryLightState.isOn = on;
        save_eeprom();
    }
}

bool isOn() {
    return lightState.isOn;
}

/* in app_ledgroups.cpp */
void app_ledgroups_setup();

void led_setup() {
    strip.Begin();

    Serial.printf("Loading app %s\n", eepromContent.app);
    if (!setApp(eepromContent.app, false)) {
        setApp("cycle");
    }


    setCurrentLimit(eepromContent.currentLimit, false);
    setOn(eepromContent.primaryLightState.isOn, false, false);
    setBrightness(eepromContent.primaryLightState.brightness, false, false);
    setPrimaryColor(eepromContent.primaryLightState.color, false, false, false);

    for (unsigned int i = 0; i < LEDGroups.size(); i++) {
        setLEDGroupOn(i, eepromContent.ledGroupStates[i].isOn, false, false);
        setLEDGroupBrightness(i, eepromContent.ledGroupStates[i].brightness, false, false);
        setLEDGroupColor(i, eepromContent.ledGroupStates[i].color, false, false);
    }

    app_ledgroups_setup();
}

void updateFPS() {
    unsigned long currentTime = millis();
    unsigned long delta = max(1UL, currentTime - lastFrame);
    float currentFPS = 1000.0f / delta;
    fps = 0.2 * fps + 0.8 * currentFPS;
    lastFrame = currentTime;
}

void led_loop() {
    /* Update strip brightness */
    strip.SetBrightness(_brightness * MAX_BRIGHTNESS * lightState.getBrightnessFactor());
    strip.Show();

    currentApp.loop();
    updateFPS();
}

const App& getApp() {
    return currentApp;
}

bool setApp(const String& name, bool save) {
    if (getApps().find(name) == getApps().end()) {
        return false;
    }

    currentApp = getApps().at(name);
    appRespectsPrimaryColor = false;

    currentApp.setup();

    if (save) {
        strncpy(eepromContent.app, name.c_str(), sizeof(((EEPROMContent*)0)->app));
        save_eeprom();
    }

    return true;
}

void setBrightness(float brightness, bool fade, bool save) {
    brightness = min(max(brightness, 0.0f), 1.0f);
    
    _brightness.animateTo(brightness, fade ? BRIGHTNESS_ANIMATION_TIME : 0);

    strip.SetBrightness(brightness * MAX_BRIGHTNESS);
    if (save) {
        eepromContent.primaryLightState.brightness = brightness;
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

void setPrimaryColor(const RgbColor& color, bool makeVisible, bool fade, bool save) {
    primaryColor.animateTo(color, fade ? COLOR_ANIMATION_TIME : 0);

    if (save) {
        eepromContent.primaryLightState.color = color;
    }

    if (makeVisible && !appRespectsPrimaryColor) {
        setApp("solid", save);
    } else if (save) {
        save_eeprom();
    }

    notify_status_change(Characteristic_Color);
}
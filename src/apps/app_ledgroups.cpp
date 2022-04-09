#include "shader.h"
#include "api.h"
#include "HomeKitNotificationRequest.h"
#include <ESP8266WiFi.h>

extern String notificationServer;

static FadingLightState ledGroupStates[LEDGroups.size()];
static AnimatedValue<RgbColor> ledGroupColors[LEDGroups.size()];
static AnimatedValue<float> ledGroupBrightnesses[LEDGroups.size()];

std::vector<HomeKitNotificationRequest> ledGroupIsOnNotifications;
std::vector<HomeKitNotificationRequest> ledGroupBrightnessNotifications;
std::vector<HomeKitNotificationRequest> ledGroupColorNotifications;

void app_ledgroups_setup() {
    for (uint8_t i = 0; i < LEDGroups.size(); i++) {
        String suffix = "-"+String(i);
        ledGroupIsOnNotifications.push_back(HomeKitNotificationRequest(suffix, "On", [i]() { return isLEDGroupOn(i) ? "true" : "false"; }));
        ledGroupBrightnessNotifications.push_back(HomeKitNotificationRequest(suffix, "Brightness", [i]() { return String((uint8_t)(getLEDGroupBrightness(i) * 100.0f)); }));
        ledGroupColorNotifications.push_back(HomeKitNotificationRequest(suffix, "Color", [i]() { return toHexString(getLEDGroupColor(i)); }));
    }
}

void notify_status_change_ledgroup(uint8_t i, Characteristic changedCharacteristics) {
    if (!WiFi.isConnected()) { return; }
    if (!notificationServer.isEmpty()) {
        if (changedCharacteristics & Characteristic_IsOn) ledGroupIsOnNotifications[i].sendUpdate();
        if (changedCharacteristics & Characteristic_Brightness) ledGroupBrightnessNotifications[i].sendUpdate();
        if (changedCharacteristics & Characteristic_Color) ledGroupColorNotifications[i].sendUpdate();
    }
}

void setLEDGroupColor(uint8_t i, const RgbColor &color, bool fade, bool save) {
    ledGroupColors[i].animateTo(color, fade ? COLOR_ANIMATION_TIME : 0);

    if (save) {
        eepromContent.ledGroupStates[i].color = color;
        save_eeprom();
    }

    notify_status_change_ledgroup(i, Characteristic_Color);
}

RgbColor getLEDGroupColor(uint8_t i) {
    return ledGroupColors[i];
}

void setLEDGroupBrightness(uint8_t i, float brightness, bool fade, bool save) {
    ledGroupBrightnesses[i].animateTo(brightness, fade ? BRIGHTNESS_ANIMATION_TIME : 0);

    if (save) {
        eepromContent.ledGroupStates[i].brightness = brightness;
        save_eeprom();
    }

    notify_status_change_ledgroup(i, Characteristic_Brightness);
}

float getLEDGroupBrightness(uint8_t i) {
    return ledGroupBrightnesses[i];
}

void setLEDGroupOn(uint8_t i, bool on, bool fade, bool save) {
    if (i > LEDGroups.size()) return;
    ledGroupStates[i].setOn(on, fade ? ON_OFF_FADE_TIME : 0);

    if (save) {
        eepromContent.ledGroupStates[i].isOn = on;
        save_eeprom();
    }

    notify_status_change_ledgroup(i, Characteristic_IsOn);
}

bool isLEDGroupOn(uint8_t i) {
    if (i > LEDGroups.size()) return false;
    return ledGroupStates[i].isOn;
}

REGISTER_SHADER_APP(ledgroups) {
    SHADER_ONCE {
        appRespectsPrimaryColor = true;
    }

    RgbColor pixelColor(0);
    float totalFactor = 0;
    
    for (unsigned int i = 0; i < LEDGroups.size(); i++) {
        RgbColor contribution(0);
        if (
            Shader::pos.x >= LEDGroups[i].minX && Shader::pos.x <= LEDGroups[i].maxX &&
            Shader::pos.y >= LEDGroups[i].minY && Shader::pos.y <= LEDGroups[i].maxY
        ) {
            contribution = ledGroupColors[i];
        } else {
            float xDist = max(Shader::pos.x - LEDGroups[i].maxX, LEDGroups[i].minX - Shader::pos.x);
            float yDist = max(Shader::pos.y - LEDGroups[i].maxY, LEDGroups[i].minY - Shader::pos.y);
            if (xDist < LEDGroups[i].glowX || yDist < LEDGroups[i].glowY) {
                // tanh(-4*(dist/glow) + 2) + 1 -> https://www.desmos.com/calculator/vacunwxz8m
                float factorX = tanh(-4 * (xDist/LEDGroups[i].glowX) + 2) + 1;
                float factorY = tanh(-4 * (yDist/LEDGroups[i].glowY) + 2) + 1;
                float factor = factorX * factorY;
                contribution = ledGroupColors[i] * factor;
                totalFactor += factor;
            }
        }

        pixelColor += contribution * ledGroupBrightnesses[i] * ledGroupStates[i].getBrightnessFactor();
    }

    return pixelColor;
}
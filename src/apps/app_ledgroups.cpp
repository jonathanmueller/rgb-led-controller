#include "shader.h"
#include "api.h"
#include "buttons.h"
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
        ledGroupColorNotifications.push_back(HomeKitNotificationRequest(suffix, "Color", [i]() { return "\"" + toHexString(getLEDGroupColor(i)) + "\""; }));
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
    brightness = min(max(brightness, 0.0f), 1.0f);

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

static bool ledGroupButtonBrightnessDirections[LEDGroups.size()];

REGISTER_SHADER_APP(ledgroups) {
    SHADER_ONCE {
        appRespectsPrimaryColor = true;

        for (uint8_t i = 0; i < LEDGroups.size(); i++) {
            const auto &ledGroupButton = buttonStates[LEDGroups[i].button];

            if (ledGroupButton.wasPressed) {
                /* Handle a single button press */
                Serial.printf("Button for LED Group %d was pressed.\n", i);

                float brightness = getLEDGroupBrightness(i);

                /* Set to on if we're either off, or if we're on, but the brightness is zero */
                bool isOn = !isLEDGroupOn(i) || brightness == 0;

                /* Toggle on or off */
                setLEDGroupOn(i, isOn);

                /* If we toggled on, but the brightness is zero, set the brightness to 100 */
                if (isOn && brightness == 0) {
                    setLEDGroupBrightness(i, 1.0f);
                }
            } else if (ledGroupButton.isBeingHeld) {

                float brightness = getLEDGroupBrightness(i);

                /* This determines the fade speed (0.5 => 2 seconds for 100%) */
                float deltaBrightness = Shader::deltaTime * 0.5;

                /* Always set to fade IN if we're at zero brightness, but only if we started holding */
                if (!ledGroupButton.wasBeingHeld && brightness == 0) {
                    ledGroupButtonBrightnessDirections[i] = true;
                }

                /* Invert delta if we're fading OUT */
                if (!ledGroupButtonBrightnessDirections[i]) {
                    deltaBrightness *= -1;
                }

                /* Update the brightness */
                setLEDGroupBrightness(i, brightness + deltaBrightness);

                Serial.printf("Fading LED Group %d to %.2f...\n", i, brightness + deltaBrightness);
            } else if (ledGroupButton.wasBeingHeld && !ledGroupButton.state) {
                Serial.printf("Button for LED Group %d was let go. Switching fade direcion...\n", i);

                /* Switch directions after holding button */
                ledGroupButtonBrightnessDirections[i] = !ledGroupButtonBrightnessDirections[i];
            }
        }

    }

    float totalFactor = 0;
    float contributionFactors[LEDGroups.size()];
    
    for (uint8_t i = 0; i < LEDGroups.size(); i++) {
        if (
            Shader::pos.x >= LEDGroups[i].minX && Shader::pos.x <= LEDGroups[i].maxX &&
            Shader::pos.y >= LEDGroups[i].minY && Shader::pos.y <= LEDGroups[i].maxY
        ) {
            contributionFactors[i] = 1;
        } else {
            float xDist = max(Shader::pos.x - LEDGroups[i].maxX, LEDGroups[i].minX - Shader::pos.x);
            float yDist = max(Shader::pos.y - LEDGroups[i].maxY, LEDGroups[i].minY - Shader::pos.y);
            if (xDist < LEDGroups[i].glowX || yDist < LEDGroups[i].glowY) {
                // (tanh(-4*(dist/glow) + 2) + 1) / 2 -> https://www.desmos.com/calculator/5ufved4thx
                float factorX = (tanh(-4 * (xDist/LEDGroups[i].glowX) + 2.0f) + 1.0f) / 2.0f;
                float factorY = (tanh(-4 * (yDist/LEDGroups[i].glowY) + 2.0f) + 1.0f) / 2.0f;
                float factor = factorX * factorY;
                contributionFactors[i] = factor;
            } else {
                contributionFactors[i] = 0;
            }
        }
        totalFactor += contributionFactors[i];
        contributionFactors[i] *= ledGroupStates[i].getBrightnessFactor() * ledGroupBrightnesses[i];
    }

    if (totalFactor == 0) {
        return RgbColor(0);
    }

    float r = 0; float g = 0; float b = 0;
    for (uint8_t i = 0; i < LEDGroups.size(); i++) {
        RgbColor groupColor = ledGroupColors[i];
        float groupFactor = contributionFactors[i] / totalFactor;
        r += groupColor.R * groupFactor;
        g += groupColor.G * groupFactor;
        b += groupColor.B * groupFactor;
    }

    if (totalFactor < 1) {
        totalFactor = NeoEase::Gamma(totalFactor);
        r *= totalFactor;
        g *= totalFactor;
        b *= totalFactor;
    }

    r = min(max(r, 0.0f), 255.0f);
    g = min(max(g, 0.0f), 255.0f);
    b = min(max(b, 0.0f), 255.0f);
    return RgbColor((uint8_t)r, (uint8_t)g, (uint8_t)b);
}
#include "led.h"
#include "app.h"
#include "util.h"
#include "corners.h"
#include <NeoPixelBus.h>
#include <tuple>

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip(PixelCount);

String currentAppName;
App currentApp;

void led_setup() {
    strip.Begin();

    Serial.printf("Loading app %s\n", eepromContent.app);
    if (!setApp(eepromContent.app)) {
        setApp("cycle");
    }
}


void led_loop() {
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

    std::get<0>(currentApp)();

    if (save) {
        strncpy(eepromContent.app, currentAppName.c_str(), sizeof(((EEPROMContent*)0)->app));
        save_eeprom();
    }

    return true;
}

void fill(const RgbColor& color) {
    for (int i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, color);
    }
}

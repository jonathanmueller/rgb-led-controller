#include "led.h"
#include "app.h"
#include "corners.h"
#include <NeoPixelBus.h>
#include <tuple>

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip(PixelCount);

String currentAppName;
App currentApp;

void led_setup() {
    strip.Begin();

    setApp("cycle");
}


void led_loop() {
    std::get<1>(currentApp)();
}

const String& getApp() {
    return currentAppName;
}

bool setApp(const String& name) {
    if (getApps().find(name) == getApps().end()) {
        return false;
    }

    currentApp = getApps().at(name);
    currentAppName = name;

    std::get<0>(currentApp)();

    return true;
}

void fill(const RgbColor& color) {
    for (int i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, color);
    }
}

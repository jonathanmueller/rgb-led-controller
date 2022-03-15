#pragma once

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <map>

typedef std::tuple<std::function<void()>, std::function<void()>> App;

std::map<String, App>& getApps();
void registerApp(const String& name, App app);

#define REGISTER_APP(name) \
    namespace App_##name { \
        void setup(); \
        void loop(); \
    }; \
    const App &app_##name = []() { App app = std::make_tuple(App_##name::setup, App_##name::loop); registerApp(#name, app); return app; }(); \
    namespace App_##name

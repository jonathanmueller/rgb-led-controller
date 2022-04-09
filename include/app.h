#pragma once

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <map>

typedef struct {
    String name;
    std::function<void()> setup;
    std::function<void()> loop;
} App;
// typedef std::tuple<std::function<void()>, std::function<void()>> App;

std::map<String, App>& getApps();

void registerApp(const App &app);
void markAppCycle();

#define REGISTER_APP(app_name) \
    namespace App_##app_name { \
        void setup(); \
        void loop(); \
    }; \
    const App &app_##app_name = []() { App app = { .name = #app_name, .setup = App_##app_name::setup, .loop = App_##app_name::loop }; registerApp(app); return app; }(); \
    namespace App_##app_name


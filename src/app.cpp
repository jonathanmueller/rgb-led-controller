#include "app.h"


std::map<String, App>& getApps() {
    static std::map<String, App> apps;
    return apps;
}

void registerApp(const String& name, App app) {
    getApps()[name] = app;
}

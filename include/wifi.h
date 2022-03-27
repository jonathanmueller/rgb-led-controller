#pragma once

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

void wifi_loop();
void wifi_setup();
#include "wifi.h"
#include "led.h"

#include <Arduino.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <LittleFS.h>

//needed for library
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager


AsyncWebServer server(80);
DNSServer dns;
AsyncWebSocket ledStatusWs("/ledStatus");


String getLEDStatusString();

void wifi_setup() {
    pinMode(D2, INPUT_PULLUP);
    pinMode(LED_BUILTIN_AUX, OUTPUT);
    digitalWrite(LED_BUILTIN_AUX, HIGH);

    WiFi.hostname("LED_Sign");
    WiFi.mode(WiFiMode::WIFI_STA);

    WiFi.begin();

    if (LittleFS.begin()) {
        server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
    } else {
        Serial.println("Could not mount LittleFS, not serving files.");
    }

    server.on("/info", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>Info</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        response->print("<style>* {font-family: sans-serif;} td { padding: 0.2em 0.5em; } td:first-child { font-weight: bold; text-align: right; }</style>");
        response->print("</head><body>");
        response->print("<h1 style=\"text-align: center;\">Info</h1><table style=\"margin:0 auto;\">");
        response->printf("<tr><td>Host name</td><td>%s</td></tr>", WiFi.hostname().c_str());
        response->printf("<tr><td>Chip ID</td><td>%08X</td></tr>", ESP.getFlashChipId());
        response->printf("<tr><td>MAC</td><td>%s</td></tr>", WiFi.macAddress().c_str());
        response->printf("<tr><td>IP Address</td><td>%s</td></tr>", WiFi.localIP().toString().c_str());
        response->printf("<tr><td>Sketch used</td><td>%d kB</td></tr>", ESP.getSketchSize() / 1024);
        response->printf("<tr><td>Sketch free</td><td>%d kB</td></tr>", ESP.getFreeSketchSpace() / 1024);
        response->printf("<tr><td>Heap free</td><td>%d kB</td></tr>", ESP.getFreeHeap() / 1024);
        response->printf("<tr><td>Core version</td><td>%s</td></tr>", ESP.getCoreVersion().c_str());
        response->printf("<tr><td>Boot version</td><td>%d</td></tr>", ESP.getBootVersion());
        response->printf("<tr><td>SDK version</td><td>%s</td></tr>", ESP.getSdkVersion());
        response->printf("<tr><td>Current draw</td><td>%u mA</td></tr>", strip.CalcTotalMilliAmpere(NeoRgbCurrentSettings(160, 160, 160)));
        response->printf("<tr><td>Current limit</td><td>"); if (getCurrentLimit() == 0) { response->printf("none"); } else { response->printf("%u mA", getCurrentLimit()); } response->printf("</td></tr>");
        response->printf("<tr><td>FPS</td><td>%.0f</td></tr>", getFPS());

        long seconds = millis() / 1000;
        int minutes = seconds / 60; seconds %= 60;
        int hours = minutes / 60; minutes %= 60;
        response->printf("<tr><td>Uptime</td><td>%dh %dm %ds</td></tr>", hours, minutes, (int)seconds);
        response->print("</table>");

        response->print("<script>const refresh = function() { setTimeout(async () => fetch(location.href).then(r => r.text()).then(r => document.documentElement.innerHTML = r).finally(() => refresh()), 1000); }; refresh();</script>");

        request->send(response);
        });

    server.on("/apps", [](AsyncWebServerRequest* request) {

        AsyncWebParameter* p = request->getParam("app", true);
        if (p) {
            setApp(p->value());
            request->redirect("/apps");
            return;
        }

        AsyncResponseStream* response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>Apps</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        response->print("<style>* {font-family: sans-serif;} ul {display: flex;flex-wrap: wrap;flex-direction: row;gap: 1em;list-style: none; padding:0; justify-content: center;} li {cursor: pointer; flex-basis: 7em; border: 1px solid black; padding: 1em;border-radius: 0.3em; font-size: 1.2em; height: 3rem;display: flex;align-items: center;justify-content: center;} li.active { background: lightblue; font-weight: bold;}</style>");
        response->print("</head><body>");
        response->print("<h1 style=\"text-align: center;\">Installed Apps</h1><ul>");
        auto apps = getApps();
        auto currentApp = getApp();
        for (auto it = apps.begin(); it != apps.end(); it++) {
            auto name = it->first;
            boolean isCurrent = currentApp == name;
            response->print("<li onclick=\"children[0].submit();\"");
            if (isCurrent) { response->print(" class=\"active\""); }
            response->print(")><form action=\"/apps\" method=\"POST\"><input type=\"hidden\" name=\"app\" value=\"" + name + "\">");
            response->print(name);
            response->print("</form></li>");
        }
        response->print("</ul>");
        request->send(response);
        });

    server.on("/config.json", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        response->write("{\"PixelPositions\":[");
        for (int i = 0; i < PixelCount; i++) {
            response->printf("[%.2f,%.2f]", PixelPositions[i].x, PixelPositions[i].y);
            if (i < PixelCount - 1) { response->print(','); }
        }

        response->write("],\"PixelStrokeOrder\":[");
        for (int i = 0; i < PixelCount; i++) {
            response->printf("%d", PixelStrokeOrder[i]);
            if (i < PixelCount - 1) { response->print(','); }
        }

        response->write("],\"PixelLetterNumbers\":[");
        for (int i = 0; i < PixelCount; i++) {
            response->printf("%d", PixelLetterNumbers[i]);
            if (i < PixelCount - 1) { response->print(','); }
        }
        response->write("]}");

        request->send(response);
        });


    ledStatusWs.onEvent([](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        if (type == WS_EVT_CONNECT) {
            client->binary(strip.Pixels(), 3 * strip.PixelCount());
        }
        });
    server.addHandler(&ledStatusWs);

    server.onNotFound([](AsyncWebServerRequest* request) {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else {
            request->send(404, "text/plain", "Not Found");
        }
        });


    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
}

long lastLEDEvent = 0;
void wifi_loop() {
    if (ledStatusWs.count() > 0) {
        ledStatusWs.binaryAll(strip.Pixels(), 3 * strip.PixelCount());
    }

    if (!digitalRead(D3)) {
        String previousApp = getApp();
        setApp("noop", false); /* Prevent other apps from updating the strip */

        setOn(true, false);
        fill(RgbColor(0, 0, 10)); /* blue means wifi */
        strip.Show();

        Serial.println("Starting config portal...");
        digitalWrite(LED_BUILTIN_AUX, LOW);

        AsyncWiFiManager wifiManager(&server, &dns);
        wifiManager.setDebugOutput(false);
        wifiManager.setAPStaticIPConfig(IPAddress(10, 0, 1, 1), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

        WiFi.persistent(false);
        WiFi.disconnect(false);
        WiFi.persistent(true);

        wifiManager.startConfigPortal(WiFi.hostname().c_str());

        Serial.println("Connecting to WiFi");

        digitalWrite(LED_BUILTIN_AUX, HIGH);
        fill(RgbColor(10, 10, 0));
        strip.Show();
        delay(100);

        setApp(previousApp);
    }
}
#include "wifi.h"
#include "led.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <LittleFS.h>

//needed for library
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager


AsyncWebServer server(80);
DNSServer dns;

void wifi_setup() {
    WiFi.hostname("LED_Sign");
    WiFi.mode(WiFiMode::WIFI_STA);

    WiFi.begin();


    if (!LittleFS.begin()) {
        Serial.println("Could not mount LittleFS, not serving files.");
        server.serveStatic("/", LittleFS, "/www/");
    }

    server.on("/apps", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>Apps</title></head><body>");

        AsyncWebParameter* p = request->getParam("app");
        if (p && !setApp(p->value())) {
            response->print("Unknown app '" + p->value() + "'");
        }

        response->print("<h2>Installed Apps</h2><ul>");
        auto apps = getApps();
        auto currentApp = getApp();
        for (auto it = apps.begin(); it != apps.end(); it++) {
            auto name = it->first;
            boolean isCurrent = currentApp == name;
            response->print("<li><a href=\"/apps?app=" + name + "\">");
            if (isCurrent) { response->print("<b>"); }
            response->print(name);
            if (isCurrent) { response->print("</b>"); }
            response->print("</a></li>");
        }
        response->print("</ul>");

        request->send(response);
        });


    server.on("/files", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("text/html");
        response->printf("<!DOCTYPE html><html><head><title>Files at %s</title></head><body>", request->url().c_str());
        response->print("<ul>");


        File root = LittleFS.open("/www", "r");
        File file = root.openNextFile();

        while (file) {
            response->print("<li>");
            response->print(file.name());
            response->print("</li>");

            file = root.openNextFile();
        }

        response->print("</ul>");

        request->send(response);
        });

    server.onNotFound([](AsyncWebServerRequest* request) {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else {
            request->send(404);
        }
        });

    server.begin();
}


void wifi_loop() {
    if (!digitalRead(0)) {
        setApp("noop"); /* Prevent other apps from updating the strip */

        fill(RgbColor(0, 0, 10)); /* blue means wifi */
        strip.Show();

        Serial.print("Local IP:");
        Serial.println(WiFi.localIP());

        AsyncWiFiManager wifiManager(&server, &dns);
        wifiManager.setDebugOutput(false);
        wifiManager.setAPStaticIPConfig(IPAddress(10, 0, 1, 1), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

        WiFi.persistent(false);
        WiFi.disconnect(true);
        WiFi.persistent(true);

        wifiManager.startConfigPortal(WiFi.hostname().c_str());

        fill(RgbColor(10, 10, 0));
        strip.Show();
    }
}
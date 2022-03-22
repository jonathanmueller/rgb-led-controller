#include "wifi.h"
#include "led.h"

#include <Arduino.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <LittleFS.h>

//needed for library
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager


AsyncWebServer server(80);
AsyncWebSocket ledStatusWs("/ledStatus");

DNSServer dns;

String getLEDStatusString();

void wifi_setup() {
    WiFi.hostname("LED_Sign");
    WiFi.mode(WiFiMode::WIFI_STA);

    WiFi.begin();


    if (LittleFS.begin()) {
        server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
    } else {
        Serial.println("Could not mount LittleFS, not serving files.");
    }

    server.on("/apps", [](AsyncWebServerRequest* request) {

        AsyncWebParameter* p = request->getParam("app", true);
        if (p) {
            setApp(p->value());
            request->redirect("/apps");
            return;
        }

        AsyncResponseStream* response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>Apps</title></head><body>");

        response->print("<h2>Installed Apps</h2><ul>");
        auto apps = getApps();
        auto currentApp = getApp();
        for (auto it = apps.begin(); it != apps.end(); it++) {
            auto name = it->first;
            boolean isCurrent = currentApp == name;
            response->print("<li><form action=\"/apps\" method=\"POST\"><input type=\"hidden\" name=\"app\" value=\"" + name + "\"><a href=\"javascript:;\" onclick=\"parentNode.submit();\">");
            if (isCurrent) { response->print("<b>"); }
            response->print(name);
            if (isCurrent) { response->print("</b>"); }
            response->print("</a></form></li>");
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
            request->send(404);
        }
        });


    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.begin();
}

long lastLEDEvent = 0;
void wifi_loop() {
    if (ledStatusWs.count() > 0) {
        ledStatusWs.binaryAll(strip.Pixels(), 3 * strip.PixelCount());
    }

    if (!digitalRead(0)) {
        setApp("noop", false); /* Prevent other apps from updating the strip */

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
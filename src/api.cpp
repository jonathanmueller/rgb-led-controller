#include "api.h"
#include "wifi.h"
#include "led.h"
#include "util.h"
#include <LittleFS.h>

#include "HomeKitNotificationRequest.h"

static String NOTIFICATION_SERVER_FILE = "/config/notificationServer.txt";
String notificationServer;

HomeKitNotificationRequest isOnNotification("On", []() { return isOn() ? "true" : "false"; });
HomeKitNotificationRequest brightnessNotification("Brightness", []() { return String((uint8_t)(getBrightness() * 100.0f)); });
HomeKitNotificationRequest colorNotification("Color", []() { return "\"" + toHexString(primaryColor) + "\""; });

void notify_status_change(Characteristic changedCharacteristics) {
    if (!WiFi.isConnected()) { return; }
    if (!notificationServer.isEmpty()) {
        if (changedCharacteristics & Characteristic_IsOn) isOnNotification.sendUpdate();
        if (changedCharacteristics & Characteristic_Brightness) brightnessNotification.sendUpdate();
        if (changedCharacteristics & Characteristic_Color) colorNotification.sendUpdate();
    }
}

void api_setup() {
    server.on("/api/config", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        response->printf("{\"leds\":{\"count\":%u,", PixelCount);
        response->write("\"positions\":[");
        for (int i = 0; i < PixelCount; i++) {
            response->printf("[%.2f,%.2f]", PixelPositions[i].x, PixelPositions[i].y);
            if (i < PixelCount - 1) { response->print(','); }
        }
        response->write("],\"strokeOrder\":[");
        for (int i = 0; i < PixelCount; i++) {
            response->printf("%d", PixelStrokeOrder[i]);
            if (i < PixelCount - 1) { response->print(','); }
        }
        response->write("],\"letterNumbers\":[");
        for (int i = 0; i < PixelCount; i++) {
            response->printf("%d", PixelLetterNumbers[i]);
            if (i < PixelCount - 1) { response->print(','); }
        }
        response->write("],\"groups\":[");
        for (unsigned int i = 0; i < LEDGroups.size(); i++) {
            response->printf("{\"minX\":%.2f,\"maxX\":%.2f,\"minY\":%.2f,\"maxY\":%.2f,\"glowX\":%.2f,\"glowY\":%.2f}", LEDGroups[i].minX, LEDGroups[i].maxX, LEDGroups[i].minY, LEDGroups[i].maxY, LEDGroups[i].glowX, LEDGroups[i].glowY);
            if (i < LEDGroups.size() - 1) { response->print(','); }
        }
        response->write("]},\"apps\":[");
        auto apps = getApps();
        for (auto it = apps.begin(); it != apps.end(); it++) {
            if (it != apps.begin()) { response->print(','); }
            response->printf("\"%s\"", it->first.c_str());
        }
        response->write("]}");

        request->send(response);
        });

    server.on("/api/state", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        response->printf("{\"app\":\"%s\",", getApp().name.c_str());
        response->printf("\"isOn\":%s,", isOn() ? "true":"false");
        response->printf("\"color\":\"%s\",", toHexString(primaryColor).c_str());
        response->printf("\"brightness\":%.2f,", getBrightness());
        response->printf("\"ledGroups:[");
        for (uint8_t i = 0; i < LEDGroups.size(); i++) {
            if (i > 0) { response->write(','); }
            response->printf("{\"isOn\":%s,", isLEDGroupOn(i) ? "true":"false");
            response->printf("\"color\":\"%s\",", toHexString(getLEDGroupColor(i)).c_str());
            response->printf("\"brightness\":%.2f", getLEDGroupBrightness(i));
            response->print('}');
        }
        response->printf("],\"fps\":%.2f,", getFPS());
        response->printf("\"uptime\":%lu,", millis());
        response->printf("\"current\":{\"limit\":%u,\"current\":%u},", getCurrentLimit(), strip.CalcTotalMilliAmpere(NeoRgbCurrentSettings(160, 160, 160)));
        response->printf("\"heap\":{\"free\":%u},", ESP.getFreeHeap());
        response->printf("\"stack\":{\"free\":%u},", ESP.getFreeContStack());
        response->printf("\"ip\":\"%s\",", WiFi.localIP().toString().c_str());
        response->printf("\"hostname\":\"%s\"", WiFi.hostname().c_str());
        response->write('}');
        request->send(response);
    });

    server.on("/api/status", [](AsyncWebServerRequest* request) {
        AsyncWebParameter* groupParam = request->getParam("g");
        if (groupParam) {
            long group = groupParam->value().toInt();
            request->send(200, "text/plain", isLEDGroupOn(group) ? "1" : "0");
        } else {
            request->send(200, "text/plain", isOn() ? "1" : "0");
        }
    });

    server.on("/api/on", [](AsyncWebServerRequest* request) {
        AsyncWebParameter* groupParam = request->getParam("g");
        if (groupParam) {
            long group = groupParam->value().toInt();
            setLEDGroupOn(group, true);
            setApp("ledgroups");
        } else {
            setOn(true);
        }
        request->send(200);
        });

    server.on("/api/off", [](AsyncWebServerRequest* request) {
        AsyncWebParameter* groupParam = request->getParam("g");
        if (groupParam) {
            long group = groupParam->value().toInt();
            setLEDGroupOn(group, false);
            setApp("ledgroups");
        } else {
            setOn(false);
        }
        request->send(200);
        });

    server.on("/api/app", [](AsyncWebServerRequest* request) {
        /* Extract app name from parameter */
        AsyncWebParameter* valueParam = request->getParam("v");
        if (valueParam) {
            setOn(true);
            if (setApp(valueParam->value())) {
                request->send(200);
            } else {
                request->send(400);
            }
        } else {
            request->send(200, "text/plain", getApp().name);
        }

        });

    server.on("/api/brightness", [](AsyncWebServerRequest* request) {
        /* Extract brightness from parameter */
        AsyncWebParameter* valueParam = request->getParam("v");
        AsyncWebParameter* groupParam = request->getParam("g");
        
        if (valueParam) {
            long value = valueParam->value().toInt();
            if (value < 0 || value > 100) {
                /* 400 Bad Request if value is out of range */
                request->send(400);
            } else {

                if (groupParam) {
                    long group = groupParam->value().toInt();
                    /* Update light group brightness */
                    setLEDGroupBrightness(group, value / 100.0f);
                    if (value > 0) { setLEDGroupOn(group, true); }
                    setApp("ledgroups");
                } else {
                    /* Update Strip brightness */
                    setBrightness(value / 100.0f);
                    if (value > 0) { setOn(true); }
                }


                request->send(200);
            }
        } else {
            float brightness;
            if (groupParam) {
                long group = groupParam->value().toInt();
                brightness = getLEDGroupBrightness(group);
            } else {
                brightness = getBrightness();
            }

            /* Get current brightness */
            request->send(200, "text/plain", String((uint8_t)(brightness * 100.0f)));
        }
        });

    server.on("/api/color", [](AsyncWebServerRequest* request) {
        /* Extract color from parameter */
        AsyncWebParameter* valueParam = request->getParam("v");
        AsyncWebParameter* groupParam = request->getParam("g");

        if (valueParam) {
            HtmlColor newColor;
            if (0 == newColor.Parse<HtmlShortColorNames>("#" + valueParam->value())) {
                /* 400 Bad Request if color cannot be parsed */
                request->send(400);
                return;
            }


            if (groupParam) {
                long group = groupParam->value().toInt();
                setLEDGroupColor(group, newColor);
                setApp("ledgroups");
            } else {
                setPrimaryColor(newColor);
            }
            request->send(200);
        } else {
            RgbColor color;
            if (groupParam) {
                long group = groupParam->value().toInt();
                color = getLEDGroupColor(group);
            } else {
                color = primaryColor;
            }
            request->send(200, "text/plain", toHexString(color));
        }
        });

    server.on("/api/currentLimit", [](AsyncWebServerRequest* request) {
        /* Extract currentLimit from parameter */
        AsyncWebParameter* valueParam = request->getParam("v");

        if (valueParam) {
            long value = valueParam->value().toInt();
            if (value < 0 || value > UINT16_MAX) {
                /* 400 Bad Request if value is out of range */
                request->send(400);
            } else {
                /* Update Strip brightness */
                setCurrentLimit(value);
                request->send(200);
            }
        } else {
            /* Get current brightness */
            request->send(200, "text/plain", String(getCurrentLimit()));
        }
        });

    server.on("/api/notificationServer", [](AsyncWebServerRequest* request) {
        /* Extract url from parameter */
        AsyncWebParameter* valueParam = request->getParam("v");

        if (valueParam) {
            notificationServer = valueParam->value();

            if (notificationServer.indexOf("://") < 0) {
                notificationServer = "http://" + notificationServer;
            }

            if (notificationServer.endsWith("/")) {
                notificationServer.remove(notificationServer.length() - 1);
            }

            strncpy(eepromContent.notificationServer, notificationServer.c_str(), sizeof(((EEPROMContent*)0)->notificationServer));
            save_eeprom();

            request->send(200);

            notify_status_change();
            for (uint8_t i = 0; i < LEDGroups.size(); i++) {
                notify_status_change_ledgroup(i);
            }
        } else {
            /* Get current url */
            request->send(200, "text/plain", notificationServer);
        }
        });

    notificationServer = eepromContent.notificationServer;
}
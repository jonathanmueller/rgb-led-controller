#include "api.h"
#include "wifi.h"
#include "led.h"
#include "util.h"
#include <LittleFS.h>

#include "HomeKitNotificationRequest.h"

static String NOTIFICATION_SERVER_FILE = "/config/notificationServer.txt";
String notificationServer;

HomeKitNotificationRequest brightnessNotification("Brightness", []() { return String((uint8_t)(getBrightness() * 100.0f)); });
HomeKitNotificationRequest isOnNotification("On", []() { return isOn() ? "true" : "false"; });
HomeKitNotificationRequest colorNotification("Color", []() {
    char hexColor[9];
    HtmlColor(primaryColor).ToNumericalString(hexColor, 9);
    return "\"" + String(hexColor + 1) + "\"";
    });

void notify_status_change(Characteristic changedCharacteristics) {
    if (!WiFi.isConnected()) { return; }
    if (!notificationServer.isEmpty()) {
        if (changedCharacteristics & Characteristic_IsOn) isOnNotification.sendUpdate();
        if (changedCharacteristics & Characteristic_Brightness) brightnessNotification.sendUpdate();
        if (changedCharacteristics & Characteristic_Color) colorNotification.sendUpdate();
    }
}

void api_setup() {
    server.on("/api/status", [](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", isOn() ? "1" : "0");
        });

    server.on("/api/on", [](AsyncWebServerRequest* request) {
        setOn(true);
        request->send(200);
        });

    server.on("/api/off", [](AsyncWebServerRequest* request) {
        setOn(false);
        request->send(200);
        });

    server.on("/api/apps", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        response->print("[");
        auto apps = getApps();
        auto currentApp = getApp();
        for (auto it = apps.begin(); it != apps.end(); it++) {
            if (it != apps.begin()) { response->print(","); }
            response->print("\"" + it->first + "\"");
        }
        response->print("]");
        request->send(response);
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
            request->send(200, "text/plain", getApp());
        }

        });

    server.on("/api/brightness", [](AsyncWebServerRequest* request) {
        /* Extract brightness from parameter */
        AsyncWebParameter* valueParam = request->getParam("v");

        if (valueParam) {
            long value = valueParam->value().toInt();
            if (value < 0 || value > 100) {
                /* 400 Bad Request if value is out of range */
                request->send(400);
            } else {
                /* Update Strip brightness */
                setBrightness(value / 100.0f);
                request->send(200);
            }
        } else {
            /* Get current brightness */
            request->send(200, "text/plain", String((uint8_t)(getBrightness() * 100.0f)));
        }
        });

    server.on("/api/color", [](AsyncWebServerRequest* request) {
        /* Extract color from parameter */
        AsyncWebParameter* valueParam = request->getParam("v");

        if (valueParam) {
            HtmlColor newColor;
            if (0 == newColor.Parse<HtmlShortColorNames>("#" + valueParam->value())) {
                /* 400 Bad Request if color cannot be parsed */
                request->send(400);
                return;
            }

            setPrimaryColor(newColor, true, true);
            request->send(200);
        } else {
            /* Get current color */
            char hexColor[9];
            HtmlColor(primaryColor).ToNumericalString(hexColor, 9);
            request->send(200, "text/plain", hexColor + 1); /* Skip the # */
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

            File notificationServerFile = LittleFS.open(NOTIFICATION_SERVER_FILE, "w");
            notificationServerFile.print(notificationServer);
            notificationServerFile.close();
            request->send(200);

            notify_status_change();
        } else {
            /* Get current url */
            request->send(200, "text/plain", notificationServer);
        }
        });

    if (LittleFS.begin()) {
        if (LittleFS.exists(NOTIFICATION_SERVER_FILE)) {
            File notificationServerFile = LittleFS.open(NOTIFICATION_SERVER_FILE, "r");
            notificationServer = notificationServerFile.readString();
            notificationServerFile.close();
        }
    }
}
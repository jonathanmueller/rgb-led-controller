#include "homekit.h"
#include "wifi.h"
#include "led.h"
#include "util.h"

void homekit_setup() {
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
        AsyncResponseStream *response = request->beginResponseStream("application/json");
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
            if (value < 0 || value > 255) {
                /* 400 Bad Request if value is out of range */
                request->send(400);
            } else {
                /* Update Strip brightness */
                setBrightness((uint8_t)value);
                request->send(200);
            }
        } else {
            /* Get current brightness */
            request->send(200, "text/plain", String(getBrightness()));
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
            char hexColor[8];
            HtmlColor(primaryColor).ToNumericalString(hexColor, 8);
            request->send(200, "text/plain", hexColor+1); /* Skip the # */
        }
    });
}
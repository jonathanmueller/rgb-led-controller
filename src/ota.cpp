#include "ota.h"

#include <ArduinoOTA.h>
#include "led.h"
#include "app.h"

void ota_setup() {
    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname("LED_Sign");

    // No authentication by default
    ArduinoOTA.setPassword("Ibims1Admin");

    ArduinoOTA.onStart([]() {
        Serial.println("Start updating " + String(ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem"));
        setApp("noop", false); /* Prevent other apps from updating the strip */

        fill(RgbColor(0));
        strip.Show();
        });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");

        fill(RgbColor(0, 255, 0));
        strip.Show();
        });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));

        float p = (float)progress / total;

        for (int i = 0; i < PixelCount; i++) {
            float d = p - ((NormalizedPixelPositions[i].x + 1.0f) / 2.0f);
            float brightness = ((tanh(d * 20.0f) + 1.0f)/2.0f) * 255;
            strip.SetPixelColor(i, RgbColor(brightness, brightness, 0));
        }
        strip.Show();
        });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
        });
    ArduinoOTA.begin();
}

void ota_loop() {
    ArduinoOTA.handle();
}
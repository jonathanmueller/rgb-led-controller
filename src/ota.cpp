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

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        Serial.println("Start updating " + String(ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem"));
        setApp("noop"); /* Prevent other apps from updating the strip */

        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, RgbColor(0));
        }
        strip.Show();
        });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");

        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, RgbColor(0, 10, 0));
        }
        strip.Show();
        });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));

        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, i < ((float)progress / total)* PixelCount ? RgbColor(10, 10, 0) : RgbColor(0));
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
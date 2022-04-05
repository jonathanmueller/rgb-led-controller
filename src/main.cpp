
#include <Arduino.h>

#include "util.h"
#include "wifi.h"
#include "api.h"
#include "ota.h"
#include "led.h"


void setup() {
  Serial.begin(115200);
  Serial.println();

  util_setup();
  wifi_setup();
  ota_setup();
  api_setup();
  led_setup();

  server.begin();
}


void loop() {
  util_loop();
  wifi_loop();
  ota_loop();
  led_loop();
}

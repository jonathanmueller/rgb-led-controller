
#include <Arduino.h>

#include "util.h"
#include "eeprom_config.h"
#include "wifi.h"
#include "api.h"
#include "ota.h"
#include "led.h"

static_assert(LEDGroups.size() <= MAX_LED_GROUPS, "If more LED groups are needed, increase MAX_LED_GROUPS and add default EEPROM config in util.cpp");

void setup() {
  Serial.begin(115200);
  Serial.println();

  util_setup();
  eeprom_setup();
  wifi_setup();
  ota_setup();
  api_setup();
  led_setup();

  server.begin();
}


void loop() {
  eeprom_loop();
  wifi_loop();
  ota_loop();
  led_loop();
}

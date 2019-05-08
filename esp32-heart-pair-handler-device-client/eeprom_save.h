#ifndef EEPROM_SAVE_H
#define EEPROM_SAVE_H

#include <Arduino.h>
#include "color.h"
#include <Preferences.h>
Preferences prefs;

typedef struct {
  CRGB love_color;
  CRGB happy_color;
  CRGB sad_color;
  CRGB fear_color;
  CRGB anger_color;

  uint8_t[32] wifi_ssid;
  uint8_t[32] wifi_pass;
} settings_t;

settings_t load_settings() {
  return null;
}

void save_settings(CRGB love_color, CRGB happy_color, CRGB sad_color, CRGB fear_color, CRGB anger_color, std::string wifi_ssid, std::string wifi_pass) {
  prefs.begin("settings");
  
}

#endif

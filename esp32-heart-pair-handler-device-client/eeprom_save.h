#ifndef EEPROM_SAVE_H
#define EEPROM_SAVE_H

#include <Arduino.h>
#include "color.h"
#include <Preferences.h>
Preferences prefs;

boolean config_save_switch = false;

typedef struct {
  CRGB love_color;
  CRGB happy_color;
  CRGB sad_color;
  CRGB fear_color;
  CRGB anger_color;

  uint8_t opmode;

  //uint8_t wifi_ssid[32];
  //uint8_t wifi_pass[32];
} settings_t;

/*settings_t load_settings() {
  return NULL;
}*/

void save_settings(CRGB love_color, CRGB happy_color, CRGB sad_color, CRGB fear_color, CRGB anger_color, std::string wifi_ssid, std::string wifi_pass) {
  prefs.begin("settings");
  
}

#endif

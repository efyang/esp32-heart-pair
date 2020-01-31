#ifndef EEPROM_SAVE_H
#define EEPROM_SAVE_H

#include <Arduino.h>
#include "color.h"
#include <Preferences.h>
Preferences prefs;

boolean config_save_switch = false;

uint8_t opmode_tmp[1];
uint8_t master_brightness_tmp[1];
void save_settings(CRGB love_color, CRGB happy_color, CRGB sad_color, CRGB fear_color, CRGB anger_color, CRGB lamp_color, uint8_t opmode) {
  prefs.begin("settings", false);
  prefs.putBool("valid", true);
  prefs.putBytes("love_color", &love_color, sizeof(CRGB));
  prefs.putBytes("happy_color", &happy_color, sizeof(CRGB));
  prefs.putBytes("sad_color", &sad_color, sizeof(CRGB));
  prefs.putBytes("fear_color", &fear_color, sizeof(CRGB));
  prefs.putBytes("anger_color", &anger_color, sizeof(CRGB));
  prefs.putBytes("lamp_color", &lamp_color, sizeof(CRGB));
  prefs.putBool("wifi_mode", wifi_mode);
  prefs.putString("wifi_ssid", wifi_ssid.c_str());
  prefs.putString("wifi_user", wifi_user.c_str());
  prefs.putString("wifi_pass", wifi_pass.c_str());
  opmode_tmp[0] = opmode;
  prefs.putBytes("opmode", opmode_tmp, 1);
  master_brightness_tmp[0] = master_brightness;
  prefs.putBytes("brightness", master_brightness_tmp, 1);
  prefs.end();
}

CRGB load_color(const char * s) {
  uint8_t tmp[sizeof(CRGB)];
  prefs.getBytes(s, tmp ,sizeof(CRGB));
  CRGB ret = CRGB::White;
  memcpy(&ret, &tmp, sizeof(CRGB));
  return ret;
}

void load_settings() {
  prefs.begin("settings", false);
  bool hasPref = prefs.getBool("valid", false);
  if (hasPref) {
    loveColor = load_color("love_color");
    happyColor = load_color("happy_color");
    sadColor = load_color("sad_color");
    fearColor = load_color("fear_color");
    angerColor = load_color("anger_color");
    lampColor = load_color("lamp_color");
    wifi_mode = prefs.getBool("wifi_mode", 0);
    wifi_ssid = prefs.getString("wifi_ssid", "").c_str();
    wifi_user = prefs.getString("wifi_user", "").c_str();
    wifi_pass = prefs.getString("wifi_pass", "").c_str();
    prefs.getBytes("opmode", opmode_tmp, 1);
    opmode = opmode_tmp[0];
    prefs.getBytes("brightness", master_brightness_tmp, 1);
    master_brightness = master_brightness_tmp[0];
  }
  prefs.end();
}

#endif

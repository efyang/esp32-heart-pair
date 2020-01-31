#ifndef WIFI_STRING_UPDATER_H
#define WIFI_STRING_UPDATER_H

#include "wifi_setup.h"
#include <Arduino.h>

class BLEWifiStringUpdaterCallbacks: public BLECharacteristicCallbacks {
  private:
    std::string* s_ref;
    unsigned long last_write_time;
  public:
    BLEWifiStringUpdaterCallbacks(std::string* val_ref) {
      s_ref = val_ref;
      last_write_time = 0;
    }

    void onRead(BLECharacteristic* pCharacteristic) {}

    void onWrite(BLECharacteristic* pCharacteristic) {
      if (millis() - last_write_time > 1000) {
        *s_ref = pCharacteristic->getValue();
      } else {
        *s_ref += pCharacteristic->getValue();
      }
      last_write_time = millis();
      Serial.printf("\"%s\"\n", (*s_ref).c_str());
    }
};

BLECharacteristic* create_wifi_string_characteristic(BLEService *pService, std::string uuid, std::string *s) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                       uuid,
                                       BLECharacteristic::PROPERTY_WRITE
                                     );
  pCharacteristic->setValue(*s);
  pCharacteristic->setCallbacks(new BLEWifiStringUpdaterCallbacks(s));
  return pCharacteristic;
}

#endif

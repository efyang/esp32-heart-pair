#ifndef WIFI_STRING_UPDATER_H
#define WIFI_STRING_UPDATER_H

#include "wifi_setup.h"
#include <Arduino.h>

class BLEWifiStringUpdaterCallbacks: public BLECharacteristicCallbacks {
  private:
    std::string* s_ref;
  public:
    BLEWifiStringUpdaterCallbacks(std::string* val_ref) {
      s_ref = val_ref;
    }

    void onRead(BLECharacteristic* pCharacteristic) {}

    void onWrite(BLECharacteristic* pCharacteristic) {
      *s_ref = pCharacteristic->getValue();
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

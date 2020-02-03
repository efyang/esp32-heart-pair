#ifndef WIFI_CONNECT_SWITCH_H
#define WIFI_CONNECT_SWITCH_H

#include "wifi_setup.h"

class BLEWifiConnectSwitchCallbacks: public BLECharacteristicCallbacks {
  private:
    uint8_t tmp[1];
    CRGB* heart_leds;
  public:
    BLEWifiConnectSwitchCallbacks(CRGB* leds) {
      tmp[0] = 0;
      heart_leds = leds;
    }

    void onRead(BLECharacteristic* pCharacteristic) {}

    void onWrite(BLECharacteristic* pCharacteristic) {
      tmp[0] = pCharacteristic->getData()[0];
      wifi_mode = tmp[0];
      try_wifi_connect();
      blink_after_wifi_initial_setup(heart_leds);
      pCharacteristic->setValue(tmp, 0);
    }
};

BLECharacteristic* create_wifi_connect_switch_characteristic(BLEService *pService, std::string uuid, boolean s, CRGB* heart_leds) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                       uuid,
                                       BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ
                                     );
  uint8_t tmp[1];
  tmp[0] = s;
  pCharacteristic->setValue(tmp, 1);
  pCharacteristic->setCallbacks(new BLEWifiConnectSwitchCallbacks(heart_leds));
  return pCharacteristic;
}

#endif

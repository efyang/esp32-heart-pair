#ifndef WIFI_CONNECT_SWITCH_H
#define WIFI_CONNECT_SWITCH_H

#include "wifi_setup.h"

class BLEWifiConnectSwitchCallbacks: public BLECharacteristicCallbacks {
  private:
    boolean* save_switch_ref;
    uint8_t tmp[1];
  public:
    BLEWifiConnectSwitchCallbacks(boolean* val_ref) {
      save_switch_ref = val_ref;
      tmp[0] = false;
    }

    void onRead(BLECharacteristic* pCharacteristic) {}

    void onWrite(BLECharacteristic* pCharacteristic) {
      tmp[0] = true;
      pCharacteristic->setValue(tmp, 1);
      if (wifi_mode == WIFI_MODE_NORMAL) {
        setup_wifi(wifi_ssid, wifi_pass);
      } else if (wifi_mode == WIFI_MODE_EDUROAM) {
        setup_wifi_wpa2_enterprise(wifi_ssid, wifi_user, wifi_pass);
      }
      pCharacteristic->setValue(tmp, 0);
    }
};

BLECharacteristic* create_wifi_connect_switch_characteristic(BLEService *pService, std::string uuid, boolean *s) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                       uuid,
                                       BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ
                                     );
  uint8_t tmp[1];
  tmp[0] = *s;
  pCharacteristic->setValue(tmp, 1);
  pCharacteristic->setCallbacks(new BLEWifiConnectSwitchCallbacks(s));
  return pCharacteristic;
}

#endif

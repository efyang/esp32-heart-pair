#ifndef CONFIG_SAVE_SWITCH_H
#define CONFIG_SAVE_SWITCH_H

#include "eeprom_save.h"

class BLEConfigSaveSwitchCallbacks: public BLECharacteristicCallbacks {
  private:
    boolean* save_switch_ref;
    uint8_t tmp[1];
  public:
    BLEConfigSaveSwitchCallbacks(boolean* val_ref) {
      save_switch_ref = val_ref;
      tmp[0] = false;
    }

    void onRead(BLECharacteristic* pCharacteristic) {}

    void onWrite(BLECharacteristic* pCharacteristic) {
      tmp[0] = true;
      pCharacteristic->setValue(tmp, 1);
      save_settings(loveColor, happyColor, sadColor, fearColor, angerColor, lampColor, opmode);
      pCharacteristic->setValue(tmp, 0);
    }
};

BLECharacteristic* create_config_save_switch_characteristic(BLEService *pService, std::string uuid, boolean *s) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                       uuid,
                                       BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ
                                     );
  uint8_t tmp[1];
  tmp[0] = *s;
  pCharacteristic->setValue(tmp, 1);
  pCharacteristic->setCallbacks(new BLEConfigSaveSwitchCallbacks(s));
  return pCharacteristic;
}

#endif

#ifndef MODE_UPDATER_H
#define MODE_UPDATER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

class BLEModeUpdaterCallbacks: public BLECharacteristicCallbacks {
  private:
    uint8_t* mode_ref;
  public:
    BLEModeUpdaterCallbacks(uint8_t* val_ref) {
      mode_ref = val_ref;
    }

    void onRead(BLECharacteristic* pCharacteristic) {}

    void onWrite(BLECharacteristic* pCharacteristic) {
      *mode_ref = *(pCharacteristic->getData());
    }
};

BLECharacteristic * create_mode_characteristic(BLEService *pService, std::string uuid, uint8_t* opmode) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         uuid,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  uint8_t tmp[1];
  tmp[0] = *opmode;
  pCharacteristic->setValue(tmp, 1);
  pCharacteristic->setCallbacks(new BLEModeUpdaterCallbacks(opmode));
  return pCharacteristic;
}

#endif

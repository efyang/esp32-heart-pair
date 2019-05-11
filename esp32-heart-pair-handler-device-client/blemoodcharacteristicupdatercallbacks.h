#ifndef MOOD_CHAR_UPDATER_H
#define MOOD_CHAR_UPDATER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <FastLED.h>

CRGB bytesToColor(uint8_t* input) {
  return CRGB(*(input + 0), *(input + 1), *(input + 2));
}

void colorToBytes(CRGB* color_ref, uint8_t* r) {
  r[0] = color_ref->r;
  r[1] = color_ref->g;
  r[2] = color_ref->b;
}

class BLEMoodCharacteristicUpdaterCallbacks: public BLECharacteristicCallbacks {
  private:
    CRGB* color_ref;
    uint8_t tmp[3];
  public:
    BLEMoodCharacteristicUpdaterCallbacks(CRGB* val_ref) {
      color_ref = val_ref;
    }

    void onRead(BLECharacteristic* pCharacteristic) {
      colorToBytes(color_ref, tmp);
      pCharacteristic->setValue(tmp, 3);
    }

    void onWrite(BLECharacteristic* pCharacteristic) {
      *color_ref = bytesToColor(pCharacteristic->getData());
      colorToBytes(color_ref, tmp);
      pCharacteristic->setValue(tmp, 3);
    }
};

BLECharacteristic * create_mood_color_characteristic(BLEService *pService, std::string uuid, CRGB* color) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         uuid,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  uint8_t tmp[3];
  colorToBytes(color, tmp);
  pCharacteristic->setValue(tmp, 3);
  pCharacteristic->setCallbacks(new BLEMoodCharacteristicUpdaterCallbacks(color));
  return pCharacteristic;
}

#endif

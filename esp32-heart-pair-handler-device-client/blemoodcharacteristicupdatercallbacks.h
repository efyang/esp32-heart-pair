#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <FastLED.h>

std::string colorToString(CRGB* color) {
  char buffer [50];
  sprintf(buffer, "%02X%02X%02X", color->r, color->g, color->b);
  return buffer;
}

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
      Serial.println("Read value");
      Serial.println(colorToString(color_ref).c_str());
      colorToBytes(color_ref, tmp);
      pCharacteristic->setValue(tmp, 3);
    }

    void onWrite(BLECharacteristic* pCharacteristic) {
      Serial.println("Wrote value");
      *color_ref = bytesToColor(pCharacteristic->getData());
      Serial.println(colorToString(color_ref).c_str());
      colorToBytes(color_ref, tmp);
      pCharacteristic->setValue(tmp, 3);
    }
};

BLECharacteristic * create_mood_color_characteristic(BLEService *pService, std::string uuid, CRGB defaultValue, CRGB* color) {
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

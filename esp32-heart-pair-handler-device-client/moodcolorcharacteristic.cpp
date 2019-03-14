#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

#include "moodcolor.h"
#include "blecharacteristicupdatercallbacks.h"

// updater callbacks for mood characteristics
class BLEMoodCharacteristicUpdater: public BLECharacteristicUpdaterCallbacks<MoodColor> {
  public:
    BLEMoodCharacteristicUpdater (MoodColor *m): BLECharacteristicUpdaterCallbacks<MoodColor> (m) {};
    MoodColor convertBytesToValue(uint8_t* input) {
      Serial.println(*(input + 0), HEX);
      Serial.println(*(input + 1), HEX);
      Serial.println(*(input + 2), HEX);
      return MoodColor(*(input + 0), *(input + 1), *(input + 2));
    }
};

BLECharacteristic * create_mood_color_characteristic(BLEService *pService, std::string uuid, MoodColor defaultValue, MoodColor* color) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         uuid,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setValue(defaultValue.toString());
  pCharacteristic->setCallbacks(new BLEMoodCharacteristicUpdater(color));
  return pCharacteristic;
}

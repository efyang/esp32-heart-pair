#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "moodcolor.h"
#include "blecharacteristicupdatercallbacks.h"

// updater callbacks for mood characteristics
class BLEMoodCharacteristicUpdater: public BLECharacteristicUpdaterCallbacks<MoodColor> {
  public:
    BLEMoodCharacteristicUpdater (MoodColor *m) : BLECharacteristicUpdaterCallbacks<MoodColor> (m) {};
    MoodColor convertStringToValue(std::string* input) {
      return MoodColor(0, 0, 0);
    }
};

BLECharacteristic * create_mood_color_characteristic(BLEService *pService, std::string uuid, MoodColor defaultValue, MoodColor* color) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         uuid,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  //pCharacteristic->setValue(defaultValue.toString());
  BLEMoodCharacteristicUpdater cb(color);
  pCharacteristic->setCallbacks(&cb);
  return pCharacteristic;
}

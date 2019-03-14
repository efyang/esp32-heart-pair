#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "moodcolor.h"
#include "blecharacteristicupdatercallbacks.h"

// updater callbacks for mood characteristics
class BLEMoodCharacteristicUpdater: public BLECharacteristicUpdaterCallbacks<MoodColor> {
  public:
    BLEMoodCharacteristicUpdater (MoodColor *m);
    MoodColor convertBytesToValue(uint8_t* input);
};

BLECharacteristic * create_mood_color_characteristic(BLEService *pService, std::string uuid, MoodColor defaultValue, MoodColor* color);

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

template <class T>
class BLECharacteristicUpdaterCallbacks: public BLECharacteristicCallbacks {
  private:
    T* value_ref;
  public:
    BLECharacteristicUpdaterCallbacks(T* val_ref) {
      value_ref = val_ref;
    }

    void onRead(BLECharacteristic* pCharacteristic) {}

    void onWrite(BLECharacteristic* pCharacteristic) {
      *value_ref = convertStringToValue(pCharacteristic->getValue());
    }

    T convertStringToValue(std::string input) {}
};

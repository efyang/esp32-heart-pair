#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

template <class T>
class BLECharacteristicUpdaterCallbacks: public BLECharacteristicCallbacks {
  private:
    T* value_ref;
  public:
    BLECharacteristicUpdaterCallbacks(T* val_ref) {
      value_ref = val_ref;
    }

    void onRead(BLECharacteristic* pCharacteristic) {
      Serial.println("Read value");
      pCharacteristic->setValue(value_ref->toString());
    }

    void onWrite(BLECharacteristic* pCharacteristic) {
      Serial.println("Wrote value");
      Serial.println(pCharacteristic->getValue().c_str());
      *value_ref = convertStringToValue(pCharacteristic->getValue());
      pCharacteristic->setValue(value_ref->toString());
    }

    T convertStringToValue(std::string input) {}
};

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

template <class T>
class BLECharacteristicUpdaterCallbacks: public BLECharacteristicCallbacks {
  private:
    T* value_ref;
    uint8_t tmp[3];
  public:
    BLECharacteristicUpdaterCallbacks(T* val_ref) {
      value_ref = val_ref;
    }

    void onRead(BLECharacteristic* pCharacteristic) {
      Serial.println("Read value");
      value_ref->toBytes(tmp);
      pCharacteristic->setValue(tmp, value_ref->bytesSize);
    }

    void onWrite(BLECharacteristic* pCharacteristic) {
      Serial.println("Wrote value");
      Serial.println(pCharacteristic->getValue().c_str());
      *value_ref = convertBytesToValue(pCharacteristic->getData());
      Serial.println(value_ref->toString().c_str());
      value_ref->toBytes(tmp);
      pCharacteristic->setValue(tmp, value_ref->bytesSize);
    }

    T convertBytesToValue(uint8_t* bytes) {}
};

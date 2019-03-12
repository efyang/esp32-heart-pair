#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "moodcolorcharacteristic.h"
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "d60df0e4-8a6f-4982-bf47-dab7e3b5d119"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

void setup_ble_gatt() {
  Serial.println("Starting BLE work!");

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  MoodColor color (0,0,0);
  BLECharacteristic *pCharacteristic = create_mood_color_characteristic(pService, CHARACTERISTIC_UUID, MoodColor(0,0,0), &color);
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

#ifndef BLE_SETUP_H
#define BLE_SETUP_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

#include "blemoodcharacteristicupdatercallbacks.h"
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "d60df0e4-8a6f-4982-bf47-dab7e3b5d119"

#define HAPPY_MOOD_COLOR_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define DEFAULT_HAPPY_MOOD_COLOR CRGB(4,5,6)
CRGB happyColor = CRGB(7,8,9);

void setup_ble_gatt() {
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32 Heart Pair");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pHappyMoodCharacteristic = create_mood_color_characteristic(pService, HAPPY_MOOD_COLOR_CHARACTERISTIC_UUID, DEFAULT_HAPPY_MOOD_COLOR, &happyColor);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

#endif

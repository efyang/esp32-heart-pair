#ifndef BLE_SETUP_H
#define BLE_SETUP_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include "color.h"
#include "opmode.h"

#include "blemoodcharacteristicupdatercallbacks.h"
#include "blemodeupdatercallbacks.h"
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "d60df0e4-8a6f-4982-bf47-dab7e3b5d119"

#define LOVE_MOOD_COLOR_CHARACTERISTIC_UUID "b6b60a30-305f-4a7e-98a3-dde8d017459d"
#define HAPPY_MOOD_COLOR_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SAD_MOOD_COLOR_CHARACTERISTIC_UUID "f6845c39-075f-4b91-a505-2e16a69d3d57"
#define FEAR_MOOD_COLOR_CHARACTERISTIC_UUID "c7dc21a4-3114-4ab2-8254-ef3c91b97b32"
#define ANGER_MOOD_COLOR_CHARACTERISTIC_UUID "838fc38a-df30-42cb-9b55-2f3596dd0506"

#define LAMP_COLOR_CHARACTERISTIC_UUID "c367b354-c1cf-43d6-8c3f-24288fc231ce"

#define MODE_CHARACTERISTIC_UUID "ae2c2e59-fb28-4737-9144-7dc72d69ccf4"

void setup_ble_gatt() {
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32 Heart Pair");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pLoveMoodColorCharacteristic = create_mood_color_characteristic(pService, LOVE_MOOD_COLOR_CHARACTERISTIC_UUID, &loveColor);
  BLECharacteristic *pHappyMoodColorCharacteristic = create_mood_color_characteristic(pService, HAPPY_MOOD_COLOR_CHARACTERISTIC_UUID, &happyColor);
  BLECharacteristic *pSadMoodColorCharacteristic = create_mood_color_characteristic(pService, SAD_MOOD_COLOR_CHARACTERISTIC_UUID, &sadColor);
  BLECharacteristic *pFearMoodColorCharacteristic = create_mood_color_characteristic(pService, FEAR_MOOD_COLOR_CHARACTERISTIC_UUID, &fearColor);
  BLECharacteristic *pAngerMoodColorCharacteristic = create_mood_color_characteristic(pService, ANGER_MOOD_COLOR_CHARACTERISTIC_UUID, &angerColor);

  BLECharacteristic *pLampColorCharacteristic = create_mood_color_characteristic(pService, LAMP_COLOR_CHARACTERISTIC_UUID, &lampColor);

  BLECharacteristic *pModeCharacteristic = create_mode_characteristic(pService, MODE_CHARACTERISTIC_UUID, &opmode);

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

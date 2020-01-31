#ifndef BLE_SETUP_H
#define BLE_SETUP_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include "color.h"
#include "opmode.h"
#include "wifi_setup.h"
#include "eeprom_save.h"

#include "blemoodcharacteristicupdatercallbacks.h"
#include "blemodeupdatercallbacks.h"
#include "blewifistringupdatecallbacks.h"
#include "bleconfigsaveswitch.h"
#include "blewificonnectswitch.h"
#include "blemoodbitstringupdatercallbacks.h"
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "d60df0e4-8a6f-4982-bf47-dab7e3b5d119"

#define LOVE_MOOD_COLOR_CHARACTERISTIC_UUID "b6b60a30-305f-4a7e-98a3-dde8d017459d"
#define HAPPY_MOOD_COLOR_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SAD_MOOD_COLOR_CHARACTERISTIC_UUID "f6845c39-075f-4b91-a505-2e16a69d3d57"
#define FEAR_MOOD_COLOR_CHARACTERISTIC_UUID "c7dc21a4-3114-4ab2-8254-ef3c91b97b32"
#define ANGER_MOOD_COLOR_CHARACTERISTIC_UUID "838fc38a-df30-42cb-9b55-2f3596dd0506"

#define MOOD_BITSTRING_CHARACTERISTIC_UUID "cc6d6901-70d8-43e0-af2b-d5bd0eacf32a"
BLECharacteristic *mood_bitstring_characteristic; // int, 10 bits for local and remote

#define LAMP_COLOR_CHARACTERISTIC_UUID "c367b354-c1cf-43d6-8c3f-24288fc231ce"

#define MODE_CHARACTERISTIC_UUID "ae2c2e59-fb28-4737-9144-7dc72d69ccf4"

#define WIFI_SSID_CHARACTERISTIC_UUID "1cea50e2-c88d-4bbb-9ae3-6b637cee1041"
#define WIFI_PASS_CHARACTERISTIC_UUID "37f294c1-c9c7-48c8-b944-4b453725f8ea"
#define WIFI_USER_CHARACTERISTIC_UUID "8dead309-383e-4725-ac09-a6cbc0e5bef7"
// #define WIFI_MODE_CHARACTERISTIC_UUID "ef2d5ed0-af34-4089-8c1f-330b156bbe6b"
#define WIFI_CONNECT_SWITCH_CHARACTERISTIC_UUID "991cde05-b892-497f-ad4a-0768b59cfbba"

#define SAVE_SWITCH_CHARACTERISTIC_UUID "eb02ef6a-07cd-4bdb-babe-3375579dc9af"

#define MASTER_BRIGHTNESS_CHARACTERISTIC_UUID "69ae6147-39d8-4d0e-8a5a-12e221041015"

bool BLEDeviceConnected = false;
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      BLEDeviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      BLEDeviceConnected = false;
    }
};

void setup_ble_gatt(CRGB* heart_leds) {
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32 Heart Pair");
  // actually overflows and causes unseen errors when mtu of message is too high
  // BLEDevice::setMTU(512);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID), 200, 0);

  
  BLECharacteristic *pLoveMoodColorCharacteristic = create_mood_color_characteristic(pService, LOVE_MOOD_COLOR_CHARACTERISTIC_UUID, &loveColor);
  BLECharacteristic *pHappyMoodColorCharacteristic = create_mood_color_characteristic(pService, HAPPY_MOOD_COLOR_CHARACTERISTIC_UUID, &happyColor);
  BLECharacteristic *pSadMoodColorCharacteristic = create_mood_color_characteristic(pService, SAD_MOOD_COLOR_CHARACTERISTIC_UUID, &sadColor);
  BLECharacteristic *pFearMoodColorCharacteristic = create_mood_color_characteristic(pService, FEAR_MOOD_COLOR_CHARACTERISTIC_UUID, &fearColor);
  BLECharacteristic *pAngerMoodColorCharacteristic = create_mood_color_characteristic(pService, ANGER_MOOD_COLOR_CHARACTERISTIC_UUID, &angerColor);

  BLECharacteristic *pLampColorCharacteristic = create_mood_color_characteristic(pService, LAMP_COLOR_CHARACTERISTIC_UUID, &lampColor);

  mood_bitstring_characteristic = create_mood_bitstring_characteristic(pService, MOOD_BITSTRING_CHARACTERISTIC_UUID, generate_mood_bitstring(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

  BLECharacteristic *pModeCharacteristic = create_mode_characteristic(pService, MODE_CHARACTERISTIC_UUID, &opmode);

  BLECharacteristic *pWifiSSIDCharacteristic = create_wifi_string_characteristic(pService, WIFI_SSID_CHARACTERISTIC_UUID, &wifi_ssid);
  BLECharacteristic *pWifiPassCharacteristic = create_wifi_string_characteristic(pService, WIFI_PASS_CHARACTERISTIC_UUID, &wifi_pass);
  BLECharacteristic *pWifiUserCharacteristic = create_wifi_string_characteristic(pService, WIFI_USER_CHARACTERISTIC_UUID, &wifi_user);
  // BLECharacteristic *pWifiModeCharacteristic = create_mode_characteristic(pService, WIFI_MODE_CHARACTERISTIC_UUID, &wifi_mode);
  BLECharacteristic *pWifiConnectSwitchCharacteristic = create_wifi_connect_switch_characteristic(pService, WIFI_CONNECT_SWITCH_CHARACTERISTIC_UUID, true, heart_leds);

  BLECharacteristic *pConfigSaveSwitchCharacteristic = create_config_save_switch_characteristic(pService, SAVE_SWITCH_CHARACTERISTIC_UUID, &config_save_switch);

  BLECharacteristic *pMasterBrightnessCharacteristic = create_mode_characteristic(pService, MASTER_BRIGHTNESS_CHARACTERISTIC_UUID, &master_brightness);

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

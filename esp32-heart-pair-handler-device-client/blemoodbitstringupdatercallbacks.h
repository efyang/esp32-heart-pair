#ifndef MOOD_BITSTRING_UPDATER_H
#define MOOD_BITSTRING_UPDATER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

uint16_t generate_mood_bitstring(bool l1, bool l2, bool l3, bool l4, bool l5, bool r1, bool r2, bool r3, bool r4, bool r5) {
  return ((uint16_t)l1 << 9) |
         ((uint16_t)l2 << 8) |
         ((uint16_t)l3 << 7) |
         ((uint16_t)l4 << 6) |
         ((uint16_t)l5 << 5) |
         ((uint16_t)r1 << 4) |
         ((uint16_t)r2 << 3) |
         ((uint16_t)r3 << 2) |
         ((uint16_t)r4 << 1) |
         ((uint16_t)r5 << 0);
}

BLECharacteristic * create_mood_bitstring_characteristic(BLEService *pService, std::string uuid, uint16_t mood_bitstring) {
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         uuid,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
  pCharacteristic->setValue(mood_bitstring);
  return pCharacteristic;
}

#endif

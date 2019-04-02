#include "blemoodcharacteristicupdatercallbacks.h"
#include "BLE_setup.h"
#include "wifi_setup.h"
#include <FastLED.h>

// How many leds in your strip?
#define NUM_BUTTON_LEDS 1
#define BUTTON_LEDS_DATA_PIN 3

// Define the array of leds
CRGB button_leds[NUM_BUTTON_LEDS];

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, BUTTON_LEDS_DATA_PIN>(button_leds, NUM_BUTTON_LEDS);
  //setup_wifi();
  setup_ble_gatt();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(colorToString(&happyColor).c_str());
  button_leds[0] = happyColor;
  FastLED.show();
  delay(50);
}

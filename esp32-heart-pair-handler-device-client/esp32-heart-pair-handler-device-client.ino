#include <FastLED.h>
#include "blemoodcharacteristicupdatercallbacks.h"
#include "blemodeupdatercallbacks.h"
#include "BLE_setup.h"
#include "wifi_setup.h"
#include "touch_sensor.h"
#include "color.h"
#include "opmode.h"
#include "hsv2rgb.h"
#include "eeprom_save.h"

#define NUM_HEART_LEDS 27
#define HEART_LEDS_DATA_PIN 26

#define NUM_BUTTON_LEDS 4
#define BUTTON_LEDS_DATA_PIN 33

#define BUTTON_INPUT_0 5
#define BUTTON_INPUT_1 23
#define BUTTON_INPUT_2 19
#define BUTTON_INPUT_3 18

#define HOLD_BUTTON_INPUT 21

// Define the array of leds
CRGB button_leds[NUM_BUTTON_LEDS];
CRGB heart_leds[NUM_HEART_LEDS];

TouchSensor love_hold_sensor = TouchSensor(HOLD_BUTTON_INPUT);
TouchSensor happy_mood_sensor = TouchSensor(BUTTON_INPUT_0);
TouchSensor sad_mood_sensor = TouchSensor(BUTTON_INPUT_1);
TouchSensor fear_mood_sensor = TouchSensor(BUTTON_INPUT_2);
TouchSensor anger_mood_sensor = TouchSensor(BUTTON_INPUT_3);

void setup() {
  Serial.begin(115200);
  load_settings();
  FastLED.addLeds<WS2812B, BUTTON_LEDS_DATA_PIN, GRB>(button_leds, NUM_BUTTON_LEDS).setCorrection(CoolWhiteFluorescent);
  FastLED.addLeds<WS2812B, HEART_LEDS_DATA_PIN, GRB>(heart_leds, NUM_HEART_LEDS).setCorrection(CoolWhiteFluorescent);
  love_hold_sensor.init();
  happy_mood_sensor.init();
  sad_mood_sensor.init();
  fear_mood_sensor.init();
  anger_mood_sensor.init();
  FastLED.setBrightness(255);
  setup_wifi("MateRS", "6199c34010c8");
  setup_ble_gatt();
}

#define GT_STEP 3
bool previous_held = false;
bool previous_prom = false;
byte gt = 0;
void loop() {
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
  // put your main code here, to run repeatedly:
  love_hold_sensor.update_state();
  happy_mood_sensor.update_state();
  sad_mood_sensor.update_state();
  fear_mood_sensor.update_state();
  anger_mood_sensor.update_state();
/*  EVERY_N_MILLISECONDS( 250 ) {
    if (opmode != OFF_MODE && wifi_connected) {
      send_client_update(love_hold_sensor.pressed,
        happy_mood_sensor.latched,
        sad_mood_sensor.latched,
        fear_mood_sensor.latched,
        anger_mood_sensor.latched);
    }
    Serial.println("every n");
  }*/
  
  CHSV love_color_hsv = rgb2hsv_approximate(loveColor);
  
  int r = loveColor.r;
  int g = loveColor.g;
  int b = loveColor.b;
  int brightness;
  
  switch (opmode) {
    case NORMAL_MODE:
      button_leds[0] = ifThenColor(happy_mood_sensor.latched, happyColor);
      button_leds[1] = ifThenColor(sad_mood_sensor.latched, sadColor);
      button_leds[2] = ifThenColor(fear_mood_sensor.latched, fearColor);
      button_leds[3] = ifThenColor(anger_mood_sensor.latched, angerColor);
    
      if (previous_held == false && love_hold_sensor.pressed) {
        gt = 0;
      }
      previous_held = love_hold_sensor.pressed;
      EVERY_N_MILLISECONDS( 10 ) {
        gt += GT_STEP;
      }

      brightness = fade_in_out_2(gt);
      r = ((r * brightness) / 2.5)/255;
      g = ((g * brightness) / 2.5)/255;
      b = ((b * brightness) / 2.5)/255;
      set_all_leds(heart_leds, NUM_HEART_LEDS, ifThenColor(love_hold_sensor.pressed, CRGB(r,g,b)));
      break;
      
    case LAMP_MODE:
      set_all_leds(button_leds, NUM_BUTTON_LEDS, CRGB::Black);
      set_all_leds(heart_leds, NUM_HEART_LEDS, lampColor);
      break;
      
    case CONFIGURE_MODE:
      button_leds[0] = happyColor;
      button_leds[1] = sadColor;
      button_leds[2] = fearColor;
      button_leds[3] = angerColor;
      set_all_leds(heart_leds, NUM_HEART_LEDS, loveColor);
      break;
      
    case PROM_MODE:
      set_all_leds(button_leds, NUM_BUTTON_LEDS, CRGB::Black);
      if (previous_prom == false) {
        t = 0;
        letter = 0;
        set_all_leds(heart_leds, NUM_HEART_LEDS, CRGB::Black);
      }
      prom_loop(heart_leds, NUM_HEART_LEDS);
      break;
      
    case OFF_MODE:
    default:
      set_all_leds(heart_leds, NUM_HEART_LEDS, CRGB::Black);
      set_all_leds(button_leds, NUM_BUTTON_LEDS, CRGB::Black);
      break;
  }

  if (opmode == PROM_MODE) {
    previous_prom = true;
  } else {
    previous_prom = false;
  }

  FastLED.show();
}

CRGB ifThenColor(bool condition, CRGB color) {
  if (condition) {
    return color;
  } else {
    return CRGB::Black;
  }
}

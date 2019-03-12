#include <string>
#include <Arduino.h>
#define byte uint8_t

#include "moodcolor.h"

MoodColor::MoodColor(byte r, byte g, byte b) {
  red = r;
  green = g;
  blue = b;
}

std::string MoodColor::toString() {
  char buffer [50];
  sprintf(buffer, "%02X%02X%02X", red, green, blue);
  return buffer;
}

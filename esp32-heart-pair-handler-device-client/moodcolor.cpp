#include <string>
#define byte uint8_t

#include "moodcolor.h"

MoodColor::MoodColor(byte r, byte g, byte b) {
  red = r;
  green = g;
  blue = b;
}

std::string MoodColor::toString() {
  return "TOSTRING NOT IMPLEMENTED";
}

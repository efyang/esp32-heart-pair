#include <string>
#define byte uint8_t

#ifndef _MOODCOLOR_H_
#define _MOODCOLOR_H_

class MoodColor {
  private:
    byte red;
    byte green;
    byte blue;
  public:
    MoodColor(byte r, byte g, byte b);
    std::string toString();
};

#endif

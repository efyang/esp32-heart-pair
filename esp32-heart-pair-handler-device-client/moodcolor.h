#include <string>
#define byte uint8_t

class MoodColor {
  private:
    byte red;
    byte green;
    byte blue;
  public:
    MoodColor(byte r, byte g, byte b);
    std::string toString();
};

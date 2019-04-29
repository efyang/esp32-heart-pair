unsigned long TOUCH_DELAY_MS = 50;

class TouchSensor {
  private:
    bool let_go = false;
    unsigned long last_time = 0;
    int pin;
  public:
    bool latched = false;
    bool pressed = false;

    TouchSensor(int input_pin) {
      pin = input_pin;
    }

    void init() {
      pinMode(pin, INPUT);
    }

    void update_state() {
        pressed = digitalRead(pin);
        if (!pressed) {
          let_go = true;
        }
        if (pressed && let_go && millis() > TOUCH_DELAY_MS + last_time) {
          last_time = millis();
          latched = !latched;
          let_go = false;
        }
    }
};

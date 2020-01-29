#ifndef COLOR_H
#define COLOR_H

#define DEFAULT_LOVE_COLOR CRGB::Pink
#define DEFAULT_HAPPY_COLOR CRGB::Yellow
#define DEFAULT_SAD_COLOR CRGB::Blue
#define DEFAULT_FEAR_COLOR CRGB::Purple
#define DEFAULT_ANGER_COLOR CRGB::Red

#define DEFAULT_LAMP_COLOR CRGB::White

CRGB loveColor = DEFAULT_LOVE_COLOR;
CRGB happyColor = DEFAULT_HAPPY_COLOR;
CRGB sadColor = DEFAULT_SAD_COLOR;
CRGB fearColor = DEFAULT_FEAR_COLOR;
CRGB angerColor = DEFAULT_ANGER_COLOR;

CRGB lampColor = DEFAULT_LAMP_COLOR;

#define FRAMES_PER_SECOND 120

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

byte heartbeat(byte t) {
  byte w = 218;
  byte m = (sin8(2 * (t + w)) + sin8(t + w)) / 2;
  byte m0 = (sin8(2 * w) + sin8(w)) / 2;
  byte z = 255 * (m - m0) / (255 - 2 * m0 + 1);
  byte scaled_m = m * m / 255;
  Serial.println(scaled_m);
  return scaled_m;
}

// intensity is the proportion intensity/intensity + 1 of the ramp-up part of the graph
byte fade_in_out_intensity(byte t, byte intensity) {
  if (t < 255 / (2 * intensity)) {
    return sin8(intensity * (t - 255 / (4 * intensity)));
  } else {
    return cos8(intensity * (t - 255 / (2 * intensity)) / (2 * intensity - 1));
  }
}

byte fade_in_out_2(byte t) {
  return fade_in_out_intensity(t, 2);
}

void set_all_leds(CRGB leds[], int num_leds, CRGB value) {
  for (int i = 0; i < num_leds; i++) {
    leds[i] = value;
  }
}

void set_all_leds_between(CRGB leds[], int lo, int hi, CRGB value) {
  for (int i = lo; i < hi; i++) {
    leds[i] = value;
  }
}

void set_leds_specific(CRGB leds[], int indices[], int indices_size, CRGB value) {
  for (int i = 0; i < indices_size; i++) {
    leds[indices[i]] = value;
  }
}

int INDICES[] = {0, 1, 2, 5};
int P_INDICES[] = {6,8,14,22,21,16,10,5,4,3};
int R_INDICES[] = {6,8,14,22,21,16,10,5,4,3,19};
int O_INDICES[] = {3,4,5,6,8,14,22,21,20,19,18,12};
int M_INDICES[] = {3,4,5,6,9,15,22,21,20,19};
int QUESTION_INDICES[] = {5,6,8,14,22,21,20,17,13};

#define T_STEP 2
byte t = 0;
byte letter = 0;
void prom_loop(CRGB* leds, int num_leds) {


  // byte value = heartbeat(t);
  // byte value = fade_in_out_intensity(t, 2);
  byte value = fade_in_out_2(t);
  // Serial.println(value);

  if (t > 255 - T_STEP) {
    letter = (letter + 1) % 5;
    set_all_leds(leds, num_leds, CRGB::Black);
  }
  CRGB k = CHSV(0, 255, value);
  // once passed into the function, C loses type information and information on size of the array
  switch(letter) {
    case 0: set_leds_specific(leds, P_INDICES, ARRAY_SIZE(P_INDICES), k);
      break;
    case 1: set_leds_specific(leds, R_INDICES, ARRAY_SIZE(R_INDICES), k);
      break;
    case 2: set_leds_specific(leds, O_INDICES, ARRAY_SIZE(O_INDICES), k);
      break;
    case 3: set_leds_specific(leds, M_INDICES, ARRAY_SIZE(M_INDICES), k);
      break;
    case 4: set_leds_specific(leds, QUESTION_INDICES, ARRAY_SIZE(QUESTION_INDICES), k);
      break;
  }
  // do some periodic updates
  EVERY_N_MILLISECONDS( 5 ) {
    t += T_STEP;
  }
}

#endif

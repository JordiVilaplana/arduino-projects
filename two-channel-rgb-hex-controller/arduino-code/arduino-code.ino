/*
  16 modes dual channel RGB LED light strip controller

  created 10 July 2021
  by Jordi Vilaplana Martí

  Licensed under the EUPL v1.2 or later
****************************************
  COMPATIBILITY DISCLAIMER
  Development target: Arduino NANO

  Other models may be compatible
  Required I/O pins:
  - Analog input x6
  - Digital input (PULL UP) x4
  - PWM digital output x6
*/

#include <math.h>

// Boolean definitions
#define FALSE 0
#define TRUE 1

// Propagation sub-mode identifiers
#define MODE_DIRECT 0
#define MODE_PROPAGATE_CH1 1
#define MODE_PROPAGATE_CH2 2
#define MODE_INVERT 3

// I/O RGB/HSV channel pins
struct Channel {
  int red_hue_pin;
  int green_saturation_pin;
  int blue_value_pin;
};

// Settings input mode
//   ch1_hsv: (TRUE) Reads channel 1 in HSV mode
//            (FALSE) Reads channel 1 in RGB mode
//   ch2_hsv: (TRUE) Reads channel 2 in HSV mode
//            (FALSE) Reads channel 2 in RGB mode
//   propagation: (MODE_DIRECT) Each channel controlled in a direct way
//                (MODE_PROPAGATE_CH1) Channel 1 settings are propagated into channel 2
//                (MODE_PROPAGATE_CH2) Channel 2 settings are propagated into channel 1
//                (MODE_INVERT) Invert channel settings (IN_CH1 -> OUT_CH2 and IN_CH2 -> OUT_CH1)
struct Mode {
  unsigned char ch1_hsv; // {FALSE, TRUE}
  unsigned char ch2_hsv; // {FALSE, TRUE}
  unsigned char propagation; // {MODE_DIRECT, MODE_PROPAGATE_CH1, MODE_PROPAGATE_CH1, MODE_INVERT}
};

// Color in Red-Green-Blue values
struct RGB {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

// Color in Hue-Saturation-Value values
struct HSV {
  float hue; // [0.0, 360.0]
  float saturation; // [0.0, 1.0]
  float value; // [0.0, 1.0]
};

// Mode selector input pins
int const IN_CH1_PROPAGATE_PIN = 12;
int const IN_CH2_PROPAGATE_PIN = 14;
int const IN_CH1_HSV_PIN = 15;
int const IN_CH2_HSV_PIN = 13;

// RGB/HSV potentiometer input channel pins {R/H, G/S, B/L}
Channel const CHANNEL_IN_1 = {A5, A6, A7};
Channel const CHANNEL_IN_2 = {A2, A3, A4};

// RGB PWM output channel pins {R, G, B}
Channel const CHANNEL_OUT_1 = {6, 5, 3};
Channel const CHANNEL_OUT_2 = {11, 10, 9};

// Reads the channel propagation switch state 
// and returns it in a Mode struct value.
//
// IMPORTANT! When digital input pins are in PULL UP mode the logic is inverted:
// - LOW read value means the circuit is CLOSED (ON, logical TRUE value)
// - HIGH read value means the circuit is OPEN (OFF, logical FALSE value)
Mode readMode() {
  Mode mode = {FALSE, FALSE, MODE_DIRECT};
  
  // Read switch states
  int switch_ch1_propagate_state = digitalRead(IN_CH1_PROPAGATE_PIN);
  int switch_ch2_propagate_state = digitalRead(IN_CH2_PROPAGATE_PIN);
  int switch_ch1_hsl_state = digitalRead(IN_CH1_HSV_PIN);
  int switch_ch2_hsl_state = digitalRead(IN_CH2_HSV_PIN);

  if (switch_ch1_propagate_state == LOW && switch_ch2_propagate_state == LOW) {
    mode.propagation = MODE_INVERT;
  } else if (switch_ch1_propagate_state == LOW) {
    mode.propagation = MODE_PROPAGATE_CH1;
  } else if (switch_ch2_propagate_state == LOW) {
    mode.propagation = MODE_PROPAGATE_CH2;
  }
  
  if (switch_ch1_hsl_state == LOW) {
    mode.ch1_hsv = TRUE;
  }
  
  if (switch_ch2_hsl_state == LOW) {
    mode.ch2_hsv = TRUE;
  }

  return mode;
}

// Read channel input settings as RGB color
RGB readRGB(Channel channel) {
  RGB rgb;
  
  // Read potentiometer analog states (10 bit resolution [0, 1023])
  int red_val = analogRead(channel.red_hue_pin);
  int green_val = analogRead(channel.green_saturation_pin);
  int blue_val = analogRead(channel.blue_value_pin);

  // Convert to RGB values (Red, Green, Blue = [0, 255])
  rgb.red = (unsigned char)round((float)red_val / 4.0);
  rgb.green = (unsigned char)round((float)green_val / 4.0);
  rgb.blue = (unsigned char)round((float)blue_val / 4.0);

  return rgb;
}

// Read channel input settings as HSV color
HSV readHSV(Channel channel) {
  HSV hsv;
  
  // Read potentiometer analog states (10 bit resolution [0, 1023])
  int hue_val = analogRead(channel.red_hue_pin);
  int saturation_val = analogRead(channel.green_saturation_pin);
  int value_val = analogRead(channel.blue_value_pin);

  // Convert to HSV values (Hue = [0.0, 360.0]; Saturation, Value = [0.0, 1.0])
  hsv.hue = ((float)hue_val * 360.0) / 1024.0;
  hsv.saturation = (float)saturation_val / 1024.0;
  hsv.value = (float)value_val / 1024.0;

  return hsv;
}

// Converts HSV color to RGB
// (see https://www.programmersought.com/article/7180233406/)
RGB HSVtoRGB(HSV hsv){
  RGB rgb;

  int i = hsv.hue / 60;
  int dif = ((int)hsv.hue) % 60;

  float rgb_max = hsv.value * 2.55f;
  float rgb_min = rgb_max * (100.0f - hsv.saturation) / 100.0f;
  float rgb_adjust = (rgb_max - rgb_min) * dif / 60.0f;

  switch (i) {
    case 0: // red
      rgb.red = rgb_max;
      rgb.green = rgb_min + rgb_adjust;
      rgb.blue = rgb_min;
      break;
    case 1: // yellow
      rgb.red = rgb_max - rgb_adjust;
      rgb.green = rgb_max;
      rgb.blue = rgb_min;
      break;
    case 2: // green
      rgb.red = rgb_min;
      rgb.green = rgb_max;
      rgb.blue = rgb_min + rgb_adjust;
      break;
    case 3: // cyan
      rgb.red = rgb_min;
      rgb.green = rgb_max - rgb_adjust;
      rgb.blue = rgb_max;
      break;
    case 4: // blue
      rgb.red = rgb_min + rgb_adjust;
      rgb.green = rgb_min;
      rgb.blue = rgb_max;
      break;
    case 5: // magenta
      rgb.red = rgb_max;
      rgb.green = rgb_min;
      rgb.blue = rgb_max - rgb_adjust;
      break;
  }
  
  return rgb;
}

// Read channel input and return always a RGB color
// if input is read in HSV mode (hsv = TRUE), it is converted to RGB
RGB readInput(Channel channel, unsigned char hsv) {
  RGB input;

  if (hsv == TRUE) {
    HSV input_hsv = readHSV(channel);
    input = HSVtoRGB(input_hsv);
  } else {
    input = readRGB(channel);
  }
  
  return input;
}

// Write RGB color to output channel
void outputRGB(Channel channel, RGB rgb) {
  analogWrite(channel.red_hue_pin, rgb.red);
  analogWrite(channel.green_saturation_pin, rgb.green);
  analogWrite(channel.blue_value_pin, rgb.blue);
}

void setup() {
  // Only digital pins must be configured
  // Set mode selector input pins as INPUT_PULLUP
  pinMode(IN_CH1_PROPAGATE_PIN, INPUT_PULLUP);
  pinMode(IN_CH2_PROPAGATE_PIN, INPUT_PULLUP);
  pinMode(IN_CH1_HSV_PIN, INPUT_PULLUP);
  pinMode(IN_CH2_HSV_PIN, INPUT_PULLUP);

  // Set RGB PWM output pins for Channel 1
  pinMode(CHANNEL_OUT_1.red_hue_pin, OUTPUT);
  pinMode(CHANNEL_OUT_1.green_saturation_pin, OUTPUT);
  pinMode(CHANNEL_OUT_1.blue_value_pin, OUTPUT);

  // Set RGB PWM output pins for Channel 2
  pinMode(CHANNEL_OUT_2.red_hue_pin, OUTPUT);
  pinMode(CHANNEL_OUT_2.green_saturation_pin, OUTPUT);
  pinMode(CHANNEL_OUT_2.blue_value_pin, OUTPUT);
}

void loop() {
  Mode mode = readMode();

  RGB input_ch1 = readInput(CHANNEL_IN_1, mode.ch1_hsv);
  RGB input_ch2 = readInput(CHANNEL_IN_2, mode.ch2_hsv);

  switch (mode.propagation) {
    case MODE_DIRECT: // IN_CH1 -> OUT_CH1; IN_CH2 -> OUT_CH2
      outputRGB(CHANNEL_OUT_1, input_ch1);
      outputRGB(CHANNEL_OUT_2, input_ch2);
      break;
    case MODE_PROPAGATE_CH1: // IN_CH1 -> OUT_CH1, OUT_CH2
      outputRGB(CHANNEL_OUT_1, input_ch1);
      outputRGB(CHANNEL_OUT_2, input_ch1);
      break;
    case MODE_PROPAGATE_CH2: // IN_CH2 -> OUT_CH1, OUT_CH2
      outputRGB(CHANNEL_OUT_1, input_ch2);
      outputRGB(CHANNEL_OUT_2, input_ch2);
      break;
    case MODE_INVERT: // IN_CH1 -> OUT_CH2; IN_CH2 -> OUT_CH1
      outputRGB(CHANNEL_OUT_1, input_ch2);
      outputRGB(CHANNEL_OUT_2, input_ch1);
      break;
  }
}

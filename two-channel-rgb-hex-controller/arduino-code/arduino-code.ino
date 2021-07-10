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

struct Channel {
  int red_hue_pin;
  int green_saturation_pin;
  int blue_value_pin;
};

struct Mode {
  unsigned char ch1_hsv; // {FALSE, TRUE}
  unsigned char ch2_hsv; // {FALSE, TRUE}
  unsigned char propagation; // {MODE_DIRECT, MODE_PROPAGATE_CH1, MODE_PROPAGATE_CH1, MODE_INVERT}
};

struct RGB {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

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
// and returns it in a readable value.
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

  if (switch_ch1_propagate_state == LOW) {
    mode.propagation += MODE_PROPAGATE_CH1;
  }
  if (switch_ch2_propagate_state == LOW) {
    mode.propagation += MODE_PROPAGATE_CH2;
  }
  
  if (switch_ch1_hsl_state == LOW) {
    mode.ch1_hsv = TRUE;
  }
  
  if (switch_ch2_hsl_state == LOW) {
    mode.ch2_hsv = TRUE;
  }

  return mode;
}

RGB readRGB(Channel channel) {
  RGB rgb;
  
  // Read potentiometer analog states
  int red_val = analogRead(channel.red_hue_pin);
  int green_val = analogRead(channel.green_saturation_pin);
  int blue_val = analogRead(channel.blue_value_pin);

  // Since the analog input pins read in 10 bit resolution (0-1023)
  // and PWM output can only be written in 8 bit resolution (0-255),
  // analog read values must be divided by 4 to fit PWM output resolution.
  rgb.red = (unsigned char)round((float)red_val / 4.0);
  rgb.green = (unsigned char)round((float)green_val / 4.0);
  rgb.blue = (unsigned char)round((float)blue_val / 4.0);

  return rgb;
}

HSV readHSV(Channel channel) {
  HSV hsv;
  
  // Read potentiometer analog states
  int hue_val = analogRead(channel.red_hue_pin);
  int saturation_val = analogRead(channel.green_saturation_pin);
  int value_val = analogRead(channel.blue_value_pin);

  hsv.hue = ((float)hue_val * 360.0) / 1024.0;
  hsv.saturation = (float)saturation_val / 1024.0;
  hsv.value = (float)value_val / 1024.0;

  return hsv;
}

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

void outputRGB(Channel channel, RGB rgb) {
  // Write the values onto the PWM pins
  analogWrite(channel.red_hue_pin, rgb.red);
  analogWrite(channel.green_saturation_pin, rgb.green);
  analogWrite(channel.blue_value_pin, rgb.blue);
}

void setup() {
  // Only digital pins must be configured
  // Set channel selector input pins as INPUT_PULLUP
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
    case MODE_DIRECT:
      outputRGB(CHANNEL_OUT_1, input_ch1);
      outputRGB(CHANNEL_OUT_2, input_ch2);
      break;
    case MODE_PROPAGATE_CH1:
      outputRGB(CHANNEL_OUT_1, input_ch1);
      outputRGB(CHANNEL_OUT_2, input_ch1);
      break;
    case MODE_PROPAGATE_CH2:
      outputRGB(CHANNEL_OUT_1, input_ch2);
      outputRGB(CHANNEL_OUT_2, input_ch2);
      break;
    case MODE_INVERT:
      outputRGB(CHANNEL_OUT_1, input_ch2);
      outputRGB(CHANNEL_OUT_2, input_ch1);
      break;
  }
}

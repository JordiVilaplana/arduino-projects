/*
  Dual channel RGB LED light controller

  created 3 July 2021
  by Jordi Vilaplana Martí

  Licensed under the EUPL v1.2 or later
****************************************
  COMPATIBILITY DISCLAIMER
  Development target: Arduino NANO

  Other models may be compatible
  Required I/O pins:
  - Analog input x6
  - PWM digital output x6
*/

#include <math.h>

struct Channel {
  int pin_red;
  int pin_green;
  int pin_blue;
};

struct RGB {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

// RGB potentiometer input pins {R, G, B}
Channel const IN_1 = {A3, A4, A5};
Channel const IN_2 = {A0, A1, A2};

// RGB PWM output pins {R, G, B}
Channel const OUT_1 = {11, 10, 9};
Channel const OUT_2 = {6, 5, 3};

RGB readInput(Channel input) {
  RGB rgb;
  
  // Read potentiometer analog states
  int red_val = analogRead(input.pin_red);
  int green_val = analogRead(input.pin_green);
  int blue_val = analogRead(input.pin_blue);

  // Since the analog input pins read in 10 bit resolution (0-1023)
  // and PWM output can only be written in 8 bit resolution (0-255),
  // analog read values must be divided by 4 to fit PWM output resolution.
  rgb.red = round((float)red_val / 4.0);
  rgb.green = round((float)green_val / 4.0);
  rgb.blue = round((float)blue_val / 4.0);

  return rgb;
}

void outputRGB(Channel output, RGB rgb) {
  analogWrite(output.pin_red, rgb.red);
  analogWrite(output.pin_green, rgb.green);
  analogWrite(output.pin_blue, rgb.blue);
}

void setup() {
  // Set RGB PWM output pins for Channel 1
  pinMode(OUT_1.pin_red, OUTPUT);
  pinMode(OUT_1.pin_green, OUTPUT);
  pinMode(OUT_1.pin_blue, OUTPUT);

  // Set RGB PWM output pins for Channel 2
  pinMode(OUT_2.pin_red, OUTPUT);
  pinMode(OUT_2.pin_green, OUTPUT);
  pinMode(OUT_2.pin_blue, OUTPUT);
}

void loop() {
  RGB rgb_1 = readInput(IN_1);
  RGB rgb_2 = readInput(IN_2);

  outputRGB(OUT_1, rgb_1);
  outputRGB(OUT_2, rgb_2);
}

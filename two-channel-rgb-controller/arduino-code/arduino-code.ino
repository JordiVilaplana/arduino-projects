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

// Red-Green-Blue data struct
// This can be used for both RGB values and I/O pin reference
struct Channel {
  int pin_red;
  int pin_green;
  int pin_blue;
};

// RGB potentiometer input pins {R, G, B}
Channel const IN_1 = {A7, A6, A5};
Channel const IN_2 = {A4, A3, A2};

// RGB PWM output pins {R, G, B}
Channel const OUT_1 = {6, 5, 3};
Channel const OUT_2 = {11, 10, 9};

// Reads the analog `input` potentiometer states for each color
// and writes its value on its corresponding PWM `output' pin.
void lightRGB(Channel input, Channel output) {
  // Read potentiometer analog states
  int red_val = analogRead(input.pin_red);
  int green_val = analogRead(input.pin_green);
  int blue_val = analogRead(input.pin_blue);

  // Since the analog input pins read in 10 bit resolution (0-1023)
  // and PWM output can only be written in 8 bit resolution (0-255),
  // analog read values must be divided by 4 to fit PWM output resolution.
  red_val = round((float)red_val / 4.0);
  green_val = round((float)green_val / 4.0);
  blue_val = round((float)blue_val / 4.0);

  // Write the values onto the PWM pins
  analogWrite(output.pin_red, red_val);
  analogWrite(output.pin_green, green_val);
  analogWrite(output.pin_blue, blue_val);
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
  lightRGB(IN_1, OUT_1);
  lightRGB(IN_2, OUT_2);
}

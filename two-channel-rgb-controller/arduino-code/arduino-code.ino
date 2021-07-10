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

// RGB sub-channel identifiers
int const RED = 0;
int const GREEN = 1;
int const BLUE = 2;

// RGB potentiometer input pins {R, G, B}
int const IN_1_PIN[] = {A7, A6, A5};
int const IN_2_PIN[] = {A4, A3, A2};

// RGB PWM output pins {R, G, B}
int const OUT_1_PIN[] = {6, 5, 3};
int const OUT_2_PIN[] = {11, 10, 9};

// Reads the analog `input[]` potentiometer states for each color
// and writes its value on its corresponding PWM `output[]' pin.
void lightRGB(int input[], int output[]) {
  // Read potentiometer analog states
  int red_val = analogRead(input[RED]);
  int green_val = analogRead(input[GREEN]);
  int blue_val = analogRead(input[BLUE]);

  // Since the analog input pins read in 10 bit resolution (0-1023)
  // and PWM output can only be written in 8 bit resolution (0-255),
  // analog read values must be divided by 4 to fit PWM output resolution.
  red_val = round((float)red_val / 4.0);
  green_val = round((float)green_val / 4.0);
  blue_val = round((float)blue_val / 4.0);

  // Write the values onto the PWM pins
  analogWrite(output[RED], red_val);
  analogWrite(output[GREEN], green_val);
  analogWrite(output[BLUE], blue_val);
}

void setup() {
  // Set RGB PWM output pins for Channel 1
  pinMode(OUT_1_PIN[RED], OUTPUT);
  pinMode(OUT_1_PIN[GREEN], OUTPUT);
  pinMode(OUT_1_PIN[BLUE], OUTPUT);

  // Set RGB PWM output pins for Channel 2
  pinMode(OUT_2_PIN[RED], OUTPUT);
  pinMode(OUT_2_PIN[GREEN], OUTPUT);
  pinMode(OUT_2_PIN[BLUE], OUTPUT);
}

void loop() {
  lightRGB(IN_1_PIN, OUT_1_PIN);
  lightRGB(IN_2_PIN, OUT_2_PIN);
}

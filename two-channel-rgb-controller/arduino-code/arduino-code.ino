/*
  Dual channel RGB LED light strip controller

  created 3 July 2021
  by Jordi Vilaplana Martí

  Licensed under the EUPL v1.2 or later
****************************************
  COMPATIBILITY DISCLAIMER
  Development target: Arduino NANO

  Other models may be compatible
  Required I/O pins:
  - Analog input x6
  - Digital input (PULL UP) x2
  - PWM digital output x6
*/

#include <math.h>

// RGB sub-channel identifiers
int const RED = 0;
int const GREEN = 1;
int const BLUE = 2;

// Channel propagation values
int const PROPAGATE_CHANNEL_NONE = 0;
int const PROPAGATE_CHANNEL_1 = 1;
int const PROPAGATE_CHANNEL_2 = 2;

// RGB potentiometer input pins {R, G, B}
int const IN_1_PIN[] = {A7, A6, A5};
int const IN_2_PIN[] = {A4, A3, A2};

// Channel selector input pins
int const IN_SWITCH_1_PIN = 14;
int const IN_SWITCH_2_PIN = 15;

// RGB PWM output pins {R, G, B}
int const OUT_1_PIN[] = {6, 5, 3};
int const OUT_2_PIN[] = {11, 10, 9};

// Reads the channel propagation switch state
// and returns it in a readable value.
//
// IMPORTANT! When digital input pins are in PULL UP mode the logic is inverted:
// - LOW read value means the circuit is CLOSED (ON, logical TRUE value)
// - HIGH read value means the circuit is OPEN (OFF, logical FALSE value)
int readChannelPropagation() {
  // Read switch states
  int switch_1_state = digitalRead(IN_SWITCH_1_PIN);
  int switch_2_state = digitalRead(IN_SWITCH_2_PIN);

  // If both switches are ON function returns `PROPAGATE_CHANNEL_NONE`
  // as it's pointless to propagate each channel into each other.
  // This can be avoided using a 3 position toggle switch (as depicted in the protoboard image).
  if (switch_1_state == LOW && switch_2_state == LOW) {
    return PROPAGATE_CHANNEL_NONE;
  }
  // Return `PROPAGATE_CHANNEL_1` if only CH1 switch is ON
  if (switch_1_state == LOW) {
    return PROPAGATE_CHANNEL_1;
  }
  // Return `PROPAGATE_CHANNEL_2` if only CH2 switch is ON
  if (switch_2_state == LOW) {
    return PROPAGATE_CHANNEL_2;
  }
  // Return `PROPAGATE_CHANNEL_NONE` if both switches are OFF
  return PROPAGATE_CHANNEL_NONE;
}

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
  // Only digital pins must be configured
  // Set channel selector input pins as INPUT_PULLUP
  pinMode(IN_SWITCH_1_PIN, INPUT_PULLUP);
  pinMode(IN_SWITCH_2_PIN, INPUT_PULLUP);

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
  int channel_propagation = readChannelPropagation();

  switch (channel_propagation) {
    case PROPAGATE_CHANNEL_NONE:
      // No propagation, each channel reads its own inputs
      lightRGB(IN_1_PIN, OUT_1_PIN);
      lightRGB(IN_2_PIN, OUT_2_PIN);
      break;
    case PROPAGATE_CHANNEL_1:
      // Both channels read inputs from channel 1
      lightRGB(IN_1_PIN, OUT_1_PIN);
      lightRGB(IN_1_PIN, OUT_2_PIN);
      break;
    case PROPAGATE_CHANNEL_2:
      // Both channels read inputs from channel 2
      lightRGB(IN_2_PIN, OUT_1_PIN);
      lightRGB(IN_2_PIN, OUT_2_PIN);
      break;
  }
}

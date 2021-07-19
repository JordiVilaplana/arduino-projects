/*
  48 buttons control deck

  created 19 July 2021
  by Jordi Vilaplana Martí

  Licensed under the EUPL v1.2 or later
****************************************
  COMPATIBILITY DISCLAIMER
  Development target: Arduino LEONARDO/MICRO

  Other models with 32u4 or SAMD controller may be compatible
  Required I/O pins:
  - Digital I/O x16 (output x4, input x12)
*/

#include <Keyboard.h>

int const KEYS[] = {KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18, KEY_F19, KEY_F20, KEY_F21, KEY_F22, KEY_F23, KEY_F24};
int const MOD_KEYS[] = {0, KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_SHIFT};
int const OUT_PINS[] = {A0, A1, A2, A3};
int const IN_PINS[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

int const OUT_LENGTH = sizeof(OUT_PINS) / sizeof(OUT_PINS[0]);
int const IN_LENGTH = sizeof(IN_PINS) / sizeof(IN_PINS[0]);

int keys_state[OUT_LENGTH][IN_LENGTH];
int state;

void setup() {
  // Init `Keyboard` library
  Keyboard.begin();

  // Init output pins and set to LOW
  for (int i = 0; i < OUT_LENGTH; i++) {
    pinMode(OUT_PINS[i], OUTPUT);
    digitalWrite(OUT_PINS[i], LOW);
  }

  // Init input pins
  for (int i = 0; i < IN_LENGTH; i++) {
    pinMode(IN_PINS[i], INPUT);
  }

  // Init keys state to LOW
  for (int out = 0; out < OUT_LENGTH; out++) {
    for (int in = 0; in < IN_LENGTH; in++) {
      keys_state[out][in] = LOW;
    }
  }
}

void loop() {
  for (int out = 0; out < OUT_LENGTH; out++) {
    // Set current output pin to HIGH
    digitalWrite(OUT_PINS[out], HIGH);

    for (int in = 0; in < IN_LENGTH; in++) {
      // Read current input pin state
      state = digitalRead(IN_PINS[in]);

      // Check if state has changed
      if (state != keys_state[out][in]) {
        // State is HIGH: send keyboard press key
        // State is LOW: send keyboard release key
        if (state == HIGH) {
          // Press F-key
          Keyboard.press(KEYS[in]);
          
          // Press Mod-key (none, Ctrl, Alt, Shift)
          if (out > 0) {
            Keyboard.press(MOD_KEYS[out]);
          }
        } else {
          // Release F-key
          Keyboard.release(KEYS[in]);

          // Release Mod-key (none, Ctrl, Alt, Shift)
          if (out > 0) {
            Keyboard.release(MOD_KEYS[out]);
          }
        }

        // Save current state
        keys_state[out][in] = state;
      }
    }

    // Reset output pin state to LOW
    digitalWrite(OUT_PINS[out], LOW);
  }
}

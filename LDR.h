#ifndef LDR_H
#define LDR_H

#include "Arduino.h"

class LDR {
public:
  int analogPin;
  int threshold;
  bool lastState;             //start as "light" (1 = bright, 0 = dark)
  unsigned long lastChange;   //debounce timer

  LDR(int pin = A2, int thresh = 100) 
    : analogPin(pin), threshold(thresh), lastState(1), lastChange(0) {}

  bool lightIntensity() {
    int lightValue = analogRead(analogPin);

    //hysteresis: add ±20 margin around threshold
    bool newState = lastState;
    if (lightValue < threshold - 20) newState = 0; // dark
    if (lightValue > threshold + 20) newState = 1; // light

    // change only if stable for at least 200 ms
    if (newState != lastState && millis() - lastChange > 200) {
      lastChange = millis();
      lastState = newState;
    }

    return lastState;
  }
};

#endif
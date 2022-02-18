#include "display.h"
#include "LedControl.h"
#include "defines.h"

#define UNIT_C true
#define UNIT_F false

LedControl display = LedControl(12, 13, 10, 1);

bool displayBlinkEnabled = false;
uint16_t displayBlinkRate = 500;
uint8_t displayIntensity = MAX_INTENSITY;

extern int16_t tempC;
extern bool temperatureUnit;
extern int16_t lowAlarmValue;
extern int16_t highAlarmValue;
extern uint8_t deviceMode;

int16_t CtoF(int16_t celsius) {
  return (int16_t)((float)celsius * 1.8f) + 32.0f;
}

void initDisplay() {

  display.setScanLimit(0, 6);                // Limit display to 6 digits
  display.shutdown(0, false);                // Enable Display...
  display.setIntensity(0, displayIntensity); // Set the intensity
  display.clearDisplay(0);                   // and clear the display.

  for (uint8_t x = 0; x <= 5; x++) {
    display.setDigit(0, x, 8, false);
  }

  delay(1000);

  for (uint8_t x = 0; x <= 5; x++) {
    display.setChar(0, x, '-', false);
  }

  if (temperatureUnit == UNIT_F) {
    display.setChar(0, 5, 'F', false);
  } else {
    display.setRow(0, 5, B01001110); // Uppercase C
  }

  display.setRow(0, 4, B01100011); // Degree sign
}

void updateUnits() {
  display.setRow(0, 4, B01100011); // Degree sign
  if (temperatureUnit == UNIT_F) {
    display.setChar(0, 5, 'F', false);
  } else {
    display.setRow(0, 5, B01001110); // Uppercase C
  }
}

void updateDisplay() {
  if (temperatureUnit == UNIT_C) {
    switch (deviceMode) {
    case MODE_RUN:
      setDisplay(tempC);
      break;
    case MODE_LSET:
      display.setChar(0, 0, 'L', false);
      display.setChar(0, 1, 'o', true);
      display.setChar(0, 2, ' ', true);
      display.setChar(0, 3, ' ', false);
      setDisplay(lowAlarmValue, 2);
      break;
    case MODE_HSET:
      display.setChar(0, 0, 'H', false);
      //display.setChar(0, 1, 'i', true);
      display.setRow(0, 1, B10010000); // Lowercase i w/ dp
      display.setChar(0, 2, ' ', true);
      display.setChar(0, 3, ' ', false);
      setDisplay(highAlarmValue, 2);
      break;
    case MODE_USET:
      //display.setChar(0, 0, 'U', false);
      display.setRow(0, 0, B00111110); // Uppercase U
      display.setChar(0, 1, 'n', true);
      display.setChar(0, 2, ' ', true);
      display.setChar(0, 3, ' ', false);
      break;
    }
  } else {
    switch (deviceMode) {
    case MODE_RUN:
      setDisplay(CtoF(tempC));
      break;
    case MODE_LSET:
      display.setChar(0, 0, 'L', false);
      display.setChar(0, 1, 'o', true);
      display.setChar(0, 2, ' ', true);
      display.setChar(0, 3, ' ', false);
      setDisplay(CtoF(lowAlarmValue), 2);
      break;
    case MODE_HSET:
      display.setChar(0, 0, 'H', false);
      //display.setChar(0, 1, 'i', true);
      display.setRow(0, 1, B00010000); // Lowercase i
      display.setChar(0, 2, ' ', true);
      display.setChar(0, 3, ' ', false);
      setDisplay(CtoF(highAlarmValue), 2);
      break;
    case MODE_USET:
      //display.setChar(0, 0, 'U', false);
      display.setRow(0, 0, B00111110); // Uppercase U
      display.setChar(0, 1, 'n', true);
      display.setChar(0, 2, ' ', true);
      display.setChar(0, 3, ' ', false);
      break;
    }
  }
}

void setDisplay(int16_t value) { setDisplay(value, 0); }

void setDisplay(int16_t value, int8_t digitOffset) {

  // Serial.print("Value: ");
  // Serial.print(value);

  if (value < 0) {
    display.setChar(0, 0 + digitOffset, '-', false);
  } else {
    display.setChar(0, 0 + digitOffset, ' ', false);
  }

  int16_t hundreds = value / 100;
  int16_t tens = (value / 10) % 10;
  int16_t ones = value % 10;

  if (hundreds == 0) {
    display.setChar(0, 1 + digitOffset, ' ', false);
  } else {
    display.setDigit(0, 1 + digitOffset, hundreds, false);
  }

  if (hundreds == 0 && tens == 0) {
    display.setChar(0, 2 + digitOffset, ' ', false);
  } else {
    display.setDigit(0, 2 + digitOffset, tens, false);
  }

  display.setDigit(0, 3 + digitOffset, ones, false);

  // Serial.print("   H: ");
  // Serial.print(hundreds);
  // Serial.print("  T: ");
  // Serial.print(tens);
  // Serial.print("  Os: ");
  // Serial.println(ones);
  // Serial.println();
}

void blinkDisplay(bool enabled, uint16_t rate) {
  displayBlinkRate = rate;
  blinkDisplay(enabled);
}

void blinkDisplay(bool enabled) {
  displayBlinkEnabled = enabled;
  if (!enabled) {
    display.setIntensity(0, displayIntensity); // Restore intensity
  }
}

void handleDisplay() {

  static bool displayState;
  static uint32_t previousMillis = millis();
  if ((millis() - previousMillis > displayBlinkRate) && displayBlinkEnabled) {
    displayState = !displayState;

    if (displayState) {
      display.setIntensity(0, MAX_INTENSITY);
    } else {
      display.setIntensity(0, MIN_INTENSITY);
    }

    previousMillis = millis();
  }
}
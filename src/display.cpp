#include "display.h"
#include "LedControl.h"

#define UNIT_C true
#define UNIT_F false

LedControl display = LedControl(12, 13, 10, 1);

extern bool temperatureUnit;

void displayInit() {
  // Limit display to 6 digits
  display.setScanLimit(0, 6);
  // Enable Display...
  display.shutdown(0, false);
  // Set the brightness to a medium values...
  display.setIntensity(0, 8);
  // and clear the display.
  display.clearDisplay(0);

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
    display.setRow(0, 5, B01001110); // uppercase C
  }

  display.setRow(0, 4, B01100011); // degree sign
}

void updateUnits() {
  if (temperatureUnit == UNIT_F) {
    display.setChar(0, 5, 'F', false);
  } else {
    display.setRow(0, 5, B01001110); // uppercase C
  }
}

void setDisplay(int16_t value) {

  // Serial.print("Value: ");
  // Serial.print(value);

  if (value < 0) {
    display.setChar(0, 0, '-', false);
  } else {
    display.setChar(0, 0, ' ', false);
  }

  int16_t hundreds = value / 100;
  int16_t tens = (value / 10) % 10;
  int16_t ones = value % 10;

  if (hundreds == 0) {
    display.setChar(0, 1, ' ', false);
  } else {
    display.setDigit(0, 1, hundreds, false);
  }

  if (hundreds == 0 && tens == 0) {
    display.setChar(0, 2, ' ', false);
  } else {
    display.setDigit(0, 2, tens, false);
  }

  display.setDigit(0, 3, ones, false);

  // Serial.print("   H: ");
  // Serial.print(hundreds);
  // Serial.print("  T: ");
  // Serial.print(tens);
  // Serial.print("  Os: ");
  // Serial.println(ones);
  // Serial.println();
}
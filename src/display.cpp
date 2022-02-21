#include "display.h"
#include "LedControl.h"
#include "defines.h"

LedControl display = LedControl(12, 13, 10, 1);

bool displayBlinkEnabled = false;
uint16_t displayBlinkRate = 500;
uint8_t displayIntensity = MAX_INTENSITY;

extern float sensorTemperatureC;
extern bool temperatureUnit;
extern float lowAlarmValue;
extern float highAlarmValue;
extern uint8_t deviceMode;

uint8_t animFrames[FRAME_COUNT] = {B00100011, B01000011, B01100010, B01100001};

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
  updateDisplay();
}

void setDisplayMode() {

  display.clearDisplay(0);

  switch (deviceMode) {
  case MODE_RUN:
  case MODE_IDLE:
    updateUnits(); // Redraws the degree sign and unit
    break;
  case MODE_USET:
    display.setRow(0, 0, B00111110); // Uppercase U
    display.setChar(0, 1, 'n', true);
    display.setChar(0, 2, ' ', true);
    updateUnits();
    break;
  case MODE_LSET:
    display.setChar(0, 0, 'L', false);
    display.setChar(0, 1, 'o', true);
    updateDisplay();
    break;
  case MODE_HSET:
    display.setChar(0, 0, 'H', false);
    display.setRow(0, 1, B10000100); // Lowercase i w/ dp
    updateDisplay();
    break;
  }
}

void updateDisplay() {

  if ((deviceMode == MODE_RUN) || (deviceMode == MODE_IDLE)) {
    updateDisplayTemp(sensorTemperatureC);
  } else {
    switch (deviceMode) {

    case MODE_LSET:
      updateDisplayMode(lowAlarmValue);
      break;
    case MODE_HSET:
      updateDisplayMode(highAlarmValue);
      break;
    }
  }
}

void updateDisplayMode(float value) {

  // Serial.print("Value: ");
  // Serial.print(value);

  if (temperatureUnit == UNIT_F) {
    value = CtoF(value);
  }

  // value = value + 0.5; // Rounding up so > x.5 = 1x

  if (value < 0.0) {
    display.setChar(0, 2, '-', true);
  } else {
    display.setChar(0, 2, ' ', true);
  }

  int16_t hundreds = value / 100;
  int16_t tens = ((int16_t)value / 10) % 10;
  int16_t ones = (int16_t)value % 10;

  if (hundreds == 0) {
    display.setChar(0, 3, ' ', false);
  } else {
    display.setDigit(0, 3, hundreds, false);
  }

  if (hundreds == 0 && tens == 0) {
    display.setChar(0, 4, ' ', false);
  } else {
    display.setDigit(0, 4, tens, false);
  }

  display.setDigit(0, 5, ones, false);

  // Serial.print("   H: ");
  // Serial.print(hundreds);
  // Serial.print("  T: ");
  // Serial.print(tens);
  // Serial.print("  Os: ");
  // Serial.println(ones);
  // Serial.println();
}

void updateDisplayTemp(float value) {

  // Serial.print("Value: ");
  // Serial.print(value);

  if (temperatureUnit == UNIT_F) {
    value = CtoF(value);
  }

  // value = value + 0.5; // Rounding up so > x.5 = 1x

  if (value < 0.0) {
    display.setChar(0, 0, '-', false);
  } else {
    display.setChar(0, 0, ' ', false);
  }

  int16_t hundreds = value / 100;
  int16_t tens = ((int16_t)value / 10) % 10;
  int16_t ones = (int16_t)value % 10;

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

  static uint8_t frameCounter = 0;
  static bool displayState;
  static uint32_t previousBlinkMillis = millis();
  static uint32_t previousAnimMillis = millis();

  if ((millis() - previousBlinkMillis > displayBlinkRate) &&
      displayBlinkEnabled) {
    displayState = !displayState;

    if (displayState) {
      display.setIntensity(0, MAX_INTENSITY);
    } else {
      display.setIntensity(0, MIN_INTENSITY);
    }

    previousBlinkMillis = millis();
  }

  if ((millis() - previousAnimMillis > 100) && deviceMode == MODE_RUN) {

    // Serial.println(frameCounter);
    display.setRow(0, 4, animFrames[frameCounter]);
    frameCounter++;
    if (frameCounter >= FRAME_COUNT) {
      frameCounter = 0;
    }
    previousAnimMillis = millis();
  }
}
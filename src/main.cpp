/*

    TODO: Add alarm setting capabilities:
            - the display handling
            - the keys handling
            - (saving to eeprom) - sensor saves settings, maybe not required,
   just read from the sensor, or set a profile, something....
*/

#include "defines.h"
#include "display.h"
#include "input.h"
#include "sensors.h"
#include <Arduino.h>

uint8_t deviceMode = MODE_RUN;

void loop() {
  handleSensors();
  handleInputs();
  handleDisplay();
}

void setup() {

  Serial.begin(57600);
  delay(1000);
  Serial.println("Hello");

  initInputs();
  initSensors();

  initDisplay();

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.println();

  delay(1000);

  Serial.println("Ready.");
}

/*

    Hunting: rounding seems off somewhere, display changes before alarm toggles, let's figure out why.

*/


#include "display.h"
#include "input.h"
#include "sensors.h"
#include <Arduino.h>

void loop() {
  handleSensors();
  handleInputs();
} // Loop

void setup() {

  Serial.begin(57600);
  delay(1000);
  Serial.println("Hello");

  inputInit();
  sensorsInit();

  displayInit();

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.println();

  delay(1000);

  Serial.println("Ready.");
}

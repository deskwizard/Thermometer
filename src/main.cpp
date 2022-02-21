/*

    TODO: Fix displayed temp VS alarm trigger temps (probably some float
   crap...)
*/

#include "defines.h"
#include "display.h"
#include "input.h"
#include "sensors.h"

uint8_t deviceMode = MODE_IDLE;

float CtoF(float celsius) { return (celsius * 1.8f) + 32.0f; }

void setLED(uint8_t ledPin, bool state) {

  if (LED_INVERT == true) {
    state = !state;
  }

  if (ledPin == LED_BLUE) {
    digitalWrite(LED_BLUE, state);
  }
  // Else it's the red one
  else {
    digitalWrite(LED_RED, state);
  }
}

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

  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  setLED(LED_BLUE, OFF);
  setLED(LED_RED, OFF);

  Serial.println();

  delay(1000);

  Serial.println("Ready.");
}

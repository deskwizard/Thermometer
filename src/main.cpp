#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "display.h"

#define UNIT_C true
#define UNIT_F false

// OneWire Bus pin (requires 4.7K external pullup)
#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

bool temperatureUnit = UNIT_F; // false = F, true = C


void loop() {

  static uint32_t lastSensorRead = millis();
  if (millis() - lastSensorRead > 1000) {

    sensors.requestTemperatures();

    if (temperatureUnit == UNIT_F) {
      float tempF = sensors.getTempFByIndex(0);

      // Check if reading was successful
      if (tempF != DEVICE_DISCONNECTED_F) {
        setDisplay(tempF);
      }
      // Otherwise we have a problem
      else {
        Serial.println("Error: Could not read temperature data");
      }
    }
    // Else we are in Celcius mode
    else {
      float tempC = sensors.getTempCByIndex(0);
      if (tempC != DEVICE_DISCONNECTED_C) {
        setDisplay(tempC);
      }

      // Otherwise we have a problem
      else {
        Serial.println("Error: Could not read temperature data");
      }
    }

    lastSensorRead = millis();
  }
}

void setup() {

  Serial.begin(57600);

  displayInit();

  sensors.begin();
  sensors.setResolution(9);

  delay(1000);

  Serial.println("Hello");

}


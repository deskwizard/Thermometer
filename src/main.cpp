#include "display.h"
#include "input.h"
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define LED_BLUE 19
#define LED_RED 18

#define UNIT_C true
#define UNIT_F false

// OneWire Bus pin (requires 4.7K external pullup)
#define ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress thermometerAddr;

bool temperatureUnit = UNIT_C; // false = F, true = C
float tempF;
float tempC;
int16_t lowAlarmValue = 20;  // 40; // 104F
int16_t highAlarmValue = 23; // 60; // 140F
bool lowAlarmAcknoledged = false;
bool highAlarmAcknoledged = false;
bool lowAlarmTriggered = false;
bool highAlarmTriggered = false;

void printAlarmInfo(const DeviceAddress deviceAddress) {
  int8_t temp;
  // printAddress(deviceAddress);

  Serial.print("Low: ");
  temp = sensors.getLowAlarmTemp(deviceAddress);
  Serial.print(temp);
  Serial.print("°C (");
  Serial.print(sensors.toFahrenheit(temp));
  Serial.print("°F)  ");

  temp = sensors.getHighAlarmTemp(deviceAddress);
  Serial.print("High: ");
  Serial.print(temp);
  Serial.print("°C (");
  Serial.print(sensors.toFahrenheit(temp));
  Serial.println("°F)  ");
  Serial.println();
}

void handleAlarms() {

  if (tempC <= lowAlarmValue && !lowAlarmAcknoledged && highAlarmAcknoledged &&
      !lowAlarmTriggered) {
    Serial.println("Low Temperature Alarm");
    lowAlarmTriggered = true;
  }

  if (tempC >= highAlarmValue && !highAlarmAcknoledged && !highAlarmTriggered) {
    Serial.println("High Temperature Alarm");
    highAlarmTriggered = true;
  }
  // }

  // void handleLEDs() {
  static bool ledState;
  static uint32_t previousMillis = millis();
  if (millis() - previousMillis > 1000) {

    if (lowAlarmTriggered && !lowAlarmAcknoledged) {
      ledState = !ledState;
      digitalWrite(LED_BLUE, ledState);
    }

    if (highAlarmTriggered && !highAlarmAcknoledged) {
      ledState = !ledState;
      digitalWrite(LED_RED, ledState);
    }

    previousMillis = millis();
  }
}

void loop() {

  static uint32_t lastSensorRead = millis();
  if (millis() - lastSensorRead > 500) {

    sensors.requestTemperatures();
    tempF = sensors.getTempF(thermometerAddr);
    tempC = sensors.getTempC(thermometerAddr);

    if (temperatureUnit == UNIT_F) {

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
      if (tempC != DEVICE_DISCONNECTED_C) {
        setDisplay(tempC);
      }

      // Otherwise we have a problem
      else {
        Serial.println("Error: Could not read temperature data");
      }
    }

    handleAlarms();

    lastSensorRead = millis();
  }

  handleInputs();
 // handleLEDs();

} // Loop

void setup() {

  Serial.begin(57600);
  delay(1000);
  Serial.println("Hello");

  inputInit();

  displayInit();

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  sensors.begin();
  sensors.setResolution(9);
  if (!sensors.getAddress(thermometerAddr, 0)) {
    Serial.println("Unable to find address for Device 0");
  } else {
    printAlarmInfo(thermometerAddr);
  }

  /*****************************************************************************/

  // set alarm ranges
  Serial.println("Setting alarm temps...");
  sensors.setHighAlarmTemp(thermometerAddr, highAlarmValue);
  sensors.setLowAlarmTemp(thermometerAddr, lowAlarmValue);
  /*
    Serial.print("New alarm values - ");
    printAlarmInfo(thermometerAddr);
    */
  Serial.println();

  delay(1000);

  Serial.println("Ready.");
}

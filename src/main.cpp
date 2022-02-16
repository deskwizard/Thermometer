#include "display.h"
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define UNIT_C true
#define UNIT_F false

// OneWire Bus pin (requires 4.7K external pullup)
#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress thermometerAddr;

bool temperatureUnit = UNIT_C; // false = F, true = C
float tempF;
float tempC;
int16_t lowAlarmValue = 20;
int16_t highAlarmValue = 23;
bool lowAlarmAcknoledged = false;
bool highAlarmAcknoledged = false;

void printAlarmInfo(const DeviceAddress deviceAddress) {
  char temp;
  // printAddress(deviceAddress);

  Serial.print("Low: ");
  temp = sensors.getLowAlarmTemp(deviceAddress);
  Serial.print(temp, DEC);
  Serial.print("°C   ");
  temp = sensors.getHighAlarmTemp(deviceAddress);
  Serial.print("High: ");
  Serial.print(temp, DEC);
  Serial.println("°C");
  Serial.println();
}

// Gets called as long as there's an alarm condition, so pretty much
// constantly.... Maybe not use that handler function and just handle in loop
// with the read ?
void newAlarmHandler(const uint8_t *deviceAddress) {

  if (tempC <= lowAlarmValue && !lowAlarmAcknoledged && highAlarmAcknoledged) {
    Serial.println("Low Temperature Alarm");
    lowAlarmAcknoledged = true;
    highAlarmAcknoledged = false;
  } else if (tempC >= highAlarmValue && !highAlarmAcknoledged) {
    Serial.println("High Temperature Alarm");
    highAlarmAcknoledged = true;
  }

  /*
  Serial.println("Alarm Handler Start");
  printAlarmInfo(deviceAddress);
  //printTemp(deviceAddress);
  Serial.println();
  Serial.println("Alarm Handler Finish");
  */
}

void loop() {

  sensors.processAlarms();

  static uint32_t lastSensorRead = millis();
  if (millis() - lastSensorRead > 1000) {

    sensors.requestTemperatures();
    tempF = sensors.getTempF(thermometerAddr);
    tempC = sensors.getTempC(thermometerAddr);

    if (temperatureUnit == UNIT_F) {
      // tempF = sensors.getTempF(thermometerAddr);

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
      // tempC = sensors.getTempC(thermometerAddr);
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
  delay(1000);
  Serial.println("Hello");
  displayInit();

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

  Serial.print("New alarm values - ");
  printAlarmInfo(thermometerAddr);
  Serial.println();

  // attach alarm handler
  sensors.setAlarmHandler(&newAlarmHandler);

  delay(1000);

  Serial.println("Ready.");
}

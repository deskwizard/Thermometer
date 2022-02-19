#include "sensors.h"
#include "defines.h"
#include "display.h"
#include <DallasTemperature.h>
#include <OneWire.h>

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress thermometerAddr;

bool temperatureUnit = UNIT_C;
float sensorTemperatureC;

int16_t lowAlarmValue = 22;  // 40; // 104F
int16_t highAlarmValue = 25; // 60; // 140F
bool lowAlarmAcknoledged = false;
bool highAlarmAcknoledged = false;
bool lowAlarmTriggered = false;
bool highAlarmTriggered = false;

void printAlarmInfo(const DeviceAddress deviceAddress) {
  int8_t temp;

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


 /******** valid range is -55C - 125C *********/
void setLowAlarm(int8_t value) {
  lowAlarmValue = value;
  sensors.setLowAlarmTemp(thermometerAddr, value);
}
void setHighAlarm(int8_t value) {
  highAlarmValue = value;
  sensors.setHighAlarmTemp(thermometerAddr, value);
}

void initSensors() {

  sensors.begin();

  if (!sensors.getAddress(thermometerAddr, 0)) {
    Serial.println(F("Unable to find address for Device 0"));
  } else {
    // printAlarmInfo(thermometerAddr);

    lowAlarmValue = sensors.getLowAlarmTemp(thermometerAddr);
    highAlarmValue = sensors.getHighAlarmTemp(thermometerAddr);

    Serial.print("Current alarm values: ");
    Serial.print("    Lo: ");
    Serial.print(lowAlarmValue);
    Serial.print("  °C   Hi: ");
    Serial.print(highAlarmValue);
    Serial.println("  °C");
  }
  /*****************************************************************************/

  // set alarm ranges
  // Serial.println("Setting alarm temps...");
  // sensors.setHighAlarmTemp(thermometerAddr, highAlarmValue);
  // sensors.setLowAlarmTemp(thermometerAddr, lowAlarmValue);
  /*
    Serial.print("New alarm values - ");
    printAlarmInfo(thermometerAddr);
    */
}

void handleSensors() {

  static uint32_t lastSensorRead = millis();
  if (millis() - lastSensorRead > 500) {

    sensors.requestTemperatures();
    sensorTemperatureC = sensors.getTempC(thermometerAddr);

    // Check for "device presence"
    if (sensorTemperatureC != DEVICE_DISCONNECTED_C) {
      updateDisplay();
    }
    // Otherwise we have a problem
    else {
      Serial.println(F("Error: Could not read temperature data"));
    }

    handleAlarms();

    lastSensorRead = millis();
  }
}

void handleAlarms() {

  static bool ledState;
  static uint32_t previousMillis = millis();
  int16_t tempC = (int16_t)(sensorTemperatureC + 0.5);

  if (tempC <= lowAlarmValue && !lowAlarmAcknoledged && highAlarmAcknoledged &&
      !lowAlarmTriggered) {
    Serial.println(F("Low Temperature Alarm"));
    lowAlarmTriggered = true;
    blinkDisplay(true);
    previousMillis = previousMillis - 1000;
  }

  if (tempC >= highAlarmValue && !highAlarmAcknoledged && !highAlarmTriggered) {
    Serial.println(F("High Temperature Alarm"));
    highAlarmTriggered = true;
    blinkDisplay(true);
    previousMillis = previousMillis - 1000;
  }

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
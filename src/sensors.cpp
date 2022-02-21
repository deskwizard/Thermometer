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

float lowAlarmValue = 22.0;  // 40; // 104F
float highAlarmValue = 25.0; // 60; // 140F
bool lowAlarmAcknoledged = false;
bool highAlarmAcknoledged = false;
bool lowAlarmTriggered = false;
bool highAlarmTriggered = false;

extern uint8_t deviceMode;

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

// FIXME: handle the float stuff/rework completely?
// /******** valid range is -55C - 125C *********/
//  ....Because that's the sensors' range

// FIXME:  Having this as a int8_t messes up setting in F mode
void setLowAlarm(int8_t value) {
  // lowAlarmValue = value;
  sensors.setLowAlarmTemp(thermometerAddr, value);
}
void setHighAlarm(int8_t value) {
  // highAlarmValue = value;
  sensors.setHighAlarmTemp(thermometerAddr, value);
}

void initSensors() {

  sensors.begin();

  if (!sensors.getAddress(thermometerAddr, 0)) {
    Serial.println(F("Unable to find address for Device 0"));
  } else {
    // printAlarmInfo(thermometerAddr);

    lowAlarmValue = (float)sensors.getLowAlarmTemp(thermometerAddr);
    highAlarmValue = (float)sensors.getHighAlarmTemp(thermometerAddr);

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

      // Serial.print("Temp: ");
      // Serial.print(sensorTemperatureC);
      // Serial.print("°C (");
      // Serial.print(CtoF(sensorTemperatureC));
      // Serial.println("°F)  ");
      // Serial.println();
    }
    // Otherwise we have a problem
    else {
      Serial.println(F("Error: Could not read temperature data"));
    }

    if (deviceMode == MODE_RUN) {
      handleAlarms();
    }

    lastSensorRead = millis();
  }
}

void handleAlarms() {

  static bool ledState;
  static uint32_t previousMillis = millis();

  if (sensorTemperatureC <= lowAlarmValue && !lowAlarmAcknoledged &&
      highAlarmAcknoledged && !lowAlarmTriggered) {
    Serial.print(F("Low Temperature Alarm: "));
    Serial.print(lowAlarmValue);
    Serial.print("°C  R:");
    Serial.print((sensorTemperatureC + 0.5));
    Serial.print("°C  S:");
    Serial.print(sensorTemperatureC);
    Serial.println("°C");
    lowAlarmTriggered = true;
    blinkDisplay(true);
    setLED(LED_BLUE, ON);
  }

  if (sensorTemperatureC >= highAlarmValue && !highAlarmAcknoledged &&
      !highAlarmTriggered) {
    Serial.print(F("High Temperature Alarm: "));
    Serial.print(highAlarmValue);
    Serial.print("°C  R:");
    Serial.print((sensorTemperatureC + 0.5));
    Serial.print("°C  S:");
    Serial.print(sensorTemperatureC);
    Serial.println("°C");
    highAlarmTriggered = true;
    blinkDisplay(true);
    setLED(LED_RED, ON);
  }

  if (millis() - previousMillis > LED_FLASH_RATE) {

    if (deviceMode == MODE_RUN) {
      if (!lowAlarmAcknoledged && highAlarmAcknoledged && !lowAlarmTriggered) {
        ledState = !ledState;
        setLED(LED_BLUE, ledState);
      }

      if (!highAlarmAcknoledged && !highAlarmTriggered) {
        ledState = !ledState;
        setLED(LED_RED, ledState);
      }
    }

    previousMillis = millis();
  }
}
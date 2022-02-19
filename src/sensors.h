#pragma once

#include <stdint.h>

// OneWire Bus pin (requires 4.7K external pullup)
#define ONE_WIRE_BUS 7

void initSensors();
void handleSensors();
void handleAlarms();
void setLowAlarm(int8_t value);
void setHighAlarm(int8_t value);

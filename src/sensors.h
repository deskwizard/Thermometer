#pragma once

// OneWire Bus pin (requires 4.7K external pullup)
#define ONE_WIRE_BUS 7

void initSensors();
void handleSensors();
void handleAlarms();

#pragma once

#include <Arduino.h>

#define MAX_INTENSITY 11
#define MIN_INTENSITY 3

void initDisplay();
void setDisplay(float value);
void setDisplay(float value, uint8_t digitOffset);
void updateUnits();
void updateDisplay();
void handleDisplay();
void blinkDisplay(bool enabled);
void blinkDisplay(bool enabled, uint16_t rate);
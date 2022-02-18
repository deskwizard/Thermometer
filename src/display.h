#pragma once

#include <Arduino.h>

#define MAX_INTENSITY 11
#define MIN_INTENSITY 3

void initDisplay();
void setDisplay(int16_t value);
void updateUnits();
void updateDisplay();
void handleDisplay();
void blinkDisplay(bool enabled);
void blinkDisplay(bool enabled, uint16_t rate);
#pragma once

#include <Arduino.h>

#define MAX_INTENSITY 8

void initDisplay();
void setDisplay(int16_t value);
void updateUnits();
void updateDisplay();
void handleDisplay();
void blinkDisplay(bool enabled);
void blinkDisplay(bool enabled, uint16_t rate);
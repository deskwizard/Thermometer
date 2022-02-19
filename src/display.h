#pragma once

#include <stdint.h>

#define MAX_INTENSITY 12
#define MIN_INTENSITY 2

void initDisplay();
void setDisplay(float value);
void setDisplay(float value, uint8_t digitOffset);
void updateUnits();
void updateDisplay();
void handleDisplay();
void blinkDisplay(bool enabled);
void blinkDisplay(bool enabled, uint16_t rate);
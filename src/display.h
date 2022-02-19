#pragma once

#include <stdint.h>

#define MAX_INTENSITY 12
#define MIN_INTENSITY 2

void initDisplay();

void setDisplayMode();

void updateDisplay();
void updateDisplayTemp(float value);
void updateDisplayMode(float value);
void updateUnits();

void handleDisplay();

void blinkDisplay(bool enabled);
void blinkDisplay(bool enabled, uint16_t rate);
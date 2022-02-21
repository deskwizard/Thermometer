#pragma once

#include <Arduino.h>

#define LED_BLUE 18
#define LED_RED 19
#define LED_FLASH_RATE 500
#define LED_INVERT  true
#define OFF false
#define ON true

#define UNIT_C true
#define UNIT_F false

#define MODE_IDLE 0  // Realtime temperature
#define MODE_RUN 1  // Realtime temperature /w animation
#define MODE_USET 2 // Measurement unit
#define MODE_LSET 3 // Low alarm temp
#define MODE_HSET 4 // High alarm temp
#define MODE_LAST MODE_HSET


float CtoF(float celsius);
void setLED(uint8_t ledPin, bool state);
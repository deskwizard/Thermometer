#pragma once

#define LED_BLUE 19
#define LED_RED 18

#define UNIT_C true
#define UNIT_F false

#define MODE_RUN 1  // Display the real time temperature
#define MODE_USET 2 // Measurement unit
#define MODE_LSET 3 // Low alarm temp
#define MODE_HSET 4 // High alarm temp
#define MODE_LAST MODE_HSET

float CtoF(float celsius);

#pragma once

#define LED_BLUE 19
#define LED_RED 18

// FIXME: Encoder is a bit dicky... dirty/old encoder?
// Rotary Encoders
#define ENC_PIN PINC
#define ENC_DDR DDRC
#define ENC_PORT PORTC
#define ENC1_A PC2
#define ENC1_B PC1

// Keys
#define KEY_PIN PINC
#define KEY_DDR DDRC
#define KEY_PORT PORTC
#define KEY0 PC0
#define KEY1 PC3
#define KEY_MASK ((1 << KEY0) | (1 << KEY1))    // Always have the mask inside ()'s, else it won't work correctly


void initInputs();
void handleInputs();
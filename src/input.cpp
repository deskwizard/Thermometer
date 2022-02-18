#include "input.h"
#include "display.h"
#include <Arduino.h>

// FIXME: remove that
#define USE_INT

volatile uint8_t currentKeyState; // debounced state
volatile uint8_t key_state;       // bit x = 1: key has changed state

void initTimer() {

  // TIMER 2 for interrupt frequency 1000 Hz:
  cli(); // stop interrupts

  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2 = 0;  // initialize counter value to 0

  // set compare match register for 1000 Hz increments
  OCR2A = 249; // = 8000000 / (32 * 1000) - 1 (must be <256)

  // turn on CTC mode
  TCCR2B |= (1 << WGM21);

  // Set CS22, CS21 and CS20 bits for 32 prescaler
  TCCR2B |= (0 << CS22) | (1 << CS21) | (1 << CS20);

  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  sei(); // allow interrupts
}

void initInputs() {

  // Pins defaults to inputs, so we only need to set the pullups here
  // FIXME: use KEY_MASK instead  ?
  KEY_PORT |= (1 << KEY0) | (1 << KEY1);

  // We need to pre-load the current values,
  // otherwise it might trigger on powerup/reset
  currentKeyState = KEY_PIN & KEY_MASK;

  // Start timer
  initTimer();
}

// Function called from interrupt vector
inline void pinRead() {

  static uint8_t counter = 0xFF;

  // Mask the pin read so we only get the bits
  // we care about and ditch the others.
  uint8_t i = KEY_PIN & KEY_MASK;

  // If the current debounced keys differs from the pin read
  if (i != currentKeyState) {

    // If it's the first time around, decrement the
    // counter once for next if statement
    if (counter == 0xFF) {

#ifndef USE_INT
      Serial.println("changed");
#endif
      counter--;

      // If it's still different, shift the counter value
    } else if (counter > 0) {

      counter = counter >> 1;
#ifndef USE_INT
      Serial.println(counter);
#endif
    }

    // If the counter reaches 0, consider the key debounced and reset the
    // debounce counter
    else {
#ifndef USE_INT
      Serial.println("saved");
#endif
      key_state = i ^ currentKeyState;
      currentKeyState = i;
      counter = 0xFF;
    }
  }

  // If the pin read value changes back to the debounced state before the
  // debounce counter reaches 0, reset the debounce counter.

  else if (counter != 0xFF) {
#ifndef USE_INT
    Serial.println("reset");
#endif
    counter = 0xFF;
  }

} // pinRead

void handleKeys() {

  if (key_state != 0) { // If it's not zero, some keys have changed.

    // This is just a routine to print the value in binary with leading zeroes.
    Serial.print("      ");
    for (uint8_t mask = 0x80; mask; mask >>= 1) {
      if (mask & key_state) {
        Serial.print('1');
      } else {
        Serial.print('0');
      }
    }
    Serial.print("      ");

    // A bit that reads as 1 means that key has changed state.

    if (bitRead(key_state, KEY0)) {
      Serial.print("Key 0 - ");
      if (bitRead(currentKeyState, KEY0)) {
        Serial.println("Released");
      } else {
        Serial.println(F("Pressed"));
      }
    }
    if (bitRead(key_state, KEY1)) {
      Serial.print("Key 1 - ");
      Serial.println(bitRead(currentKeyState, KEY1));
    }

    // all keys have been checked, reset key_state
    key_state = 0;
  }
}

void handleInputs() {
  handleKeys(); // React to debounced button(s)
  // read_encoders();
}

ISR(TIMER2_COMPA_vect) {

  // Key(s) debouncing function
  pinRead();

  // currentEnc1Pos = getEnc1Pos();
}
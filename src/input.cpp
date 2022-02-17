#include "input.h"
#include <Arduino.h>

// TODO: Key release "event"

// FIXME: Encoder is a bit dicky... dirty/old encoder?
// Rotary Encoders
#define ENC_PIN PINC
#define ENC_DDR DDRC
#define ENC_PORT PORTC
#define ENC1_A PC2
#define ENC1_B PC1

volatile unsigned char currentEnc1Pos = 0;
unsigned char lastEnc1Pos = 0;

// Keys
#define KEY_PIN PINC
#define KEY_DDR DDRC
#define KEY_PORT PORTC
#define KEY0 PC0
#define KEY1 PC3

// Debouncing variables
unsigned char
    key_state; // Debounced and inverted key state: bit = 1: key pressed
volatile unsigned char key_press;     // Key press detect
unsigned char ct0 = 0xFF, ct1 = 0xFF; // Internal debouncing states

inline uint8_t getEnc1Pos(void) {

  uint8_t enc1Pos = 0;

  if (!bit_is_clear(ENC_PIN, ENC1_A))
    enc1Pos |= (1 << 1);

  if (!bit_is_clear(ENC_PIN, ENC1_B))
    enc1Pos |= (1 << 0);

  return enc1Pos;
}

unsigned char get_key_press(unsigned char key_mask) {
  cli();
  key_mask &= key_press; // read key(s)
  key_press ^= key_mask; // clear key(s)
  sei();
  return key_mask;
}

void timerInit() {

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

void inputInit() {

  ENC_PORT |= (1 << ENC1_A) | (1 << ENC1_B); // PC1 and PC2 Input w/ pull-up

  // Preload encoder position
  currentEnc1Pos = getEnc1Pos();
  lastEnc1Pos = currentEnc1Pos;

  KEY_PORT |= (1 << KEY0) | (1 << KEY1); // PC0 and PC3 Input w/ pull-up

  // Preload debounce variable
  key_state = ~KEY_PIN; // No action on keypress during reset

  timerInit();
}

void read_keys() {

  if (get_key_press(1 << KEY0)) {
    Serial.println("KEY0");
  } // Key0

  if (get_key_press(1 << KEY1)) {
    Serial.println("KEY1");
  } // Key1

} // debounce

void read_encoders() {
  if (currentEnc1Pos != lastEnc1Pos) {

    if ((currentEnc1Pos == 3 && lastEnc1Pos == 1) ||
        (currentEnc1Pos == 0 && lastEnc1Pos == 2)) {
      Serial.println(F("1 +"));
    } else if ((currentEnc1Pos == 2 && lastEnc1Pos == 0) ||
               (currentEnc1Pos == 1 && lastEnc1Pos == 3)) {
      Serial.println(F("1 -"));
    }

    lastEnc1Pos = currentEnc1Pos;
  }
}

ISR(TIMER2_COMPA_vect) {

  // Key(s) debouncing routine
  unsigned char i;

  i = key_state ^ ~KEY_PIN;   // key changed ?
  ct0 = ~(ct0 & i);           // reset or count ct0
  ct1 = ct0 ^ (ct1 & i);      // reset or count ct1
  i &= ct0 & ct1;             // count until roll over ?
  key_state ^= i;             // then toggle debounced state
  key_press |= key_state & i; // 0 > 1: key press detect

  currentEnc1Pos = getEnc1Pos();
  // currentEnc2Pos = getEnc2Pos();
}

void handleInputs() {
  read_keys(); // React to debounced button(s)
  read_encoders();
}
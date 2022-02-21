#include "input.h"
#include "defines.h"
#include "display.h"
#include "sensors.h"

volatile uint8_t currentKeyState; // debounced state
volatile uint8_t key_state;       // bit x = 1: key has changed state

volatile uint8_t currentEncoderPos = 0;
uint8_t lastEncoderPos = 0;

extern float lowAlarmValue;
extern float highAlarmValue;
extern bool lowAlarmAcknoledged;
extern bool highAlarmAcknoledged;
extern bool lowAlarmTriggered;
extern bool highAlarmTriggered;
extern bool temperatureUnit;

extern uint8_t deviceMode;

void initInputs() {
  // Enable pullups on encoder A/B pins
  ENC_PORT |= (1 << ENC_A) | (1 << ENC_B);

  // Preload encoder position
  currentEncoderPos = readEncoder();
  lastEncoderPos = currentEncoderPos;

  // Pins defaults to inputs, so we only need to set the pullups here
  KEY_PORT |= KEY_MASK;

  // We need to pre-load the current values,
  // otherwise it might trigger on powerup/reset
  currentKeyState = KEY_PIN & KEY_MASK;

  // Start timer
  initTimer();
}

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

// Function called from interrupt vector
inline void readKeys() {

  static uint8_t counter = 0xFF;

  // Mask the pin read so we only get the bits
  // we care about and ditch the others.
  uint8_t i = KEY_PIN & KEY_MASK;

  // If the current debounced keys differs from the pin read
  if (i != currentKeyState) {

    // If it's the first time around, decrement the
    // counter once for next if statement
    if (counter == 0xFF) {
      counter--;
      // If it's still different, shift the counter value
    } else if (counter > 0) {

      counter = counter >> 1;
    }

    // If the counter reaches 0, consider the key debounced and reset the
    // debounce counter
    else {
      key_state = i ^ currentKeyState;
      currentKeyState = i;
      counter = 0xFF;
    }
  }

  // If the pin read value changes back to the debounced state before the
  // debounce counter reaches 0, reset the debounce counter.
  else if (counter != 0xFF) {
    counter = 0xFF;
  }

} // readKeys

void handleKeys() {

  if (key_state != 0) { // If it's not zero, some keys have changed.

    // This is just a routine to print the value in binary with leading zeroes.
    // Serial.print("      ");
    // for (uint8_t mask = 0x80; mask; mask >>= 1) {
    //   if (mask & key_state) {
    //     Serial.print('1');
    //   } else {
    //     Serial.print('0');
    //   }
    // }
    // Serial.print("      ");

    // A bit that reads as 1 means that key has changed state.

    if (bitRead(key_state, KEY0)) {
      Serial.print(F("Key 0 - "));

      if (bitRead(currentKeyState, KEY0)) {
        Serial.println("Released");
      } else {
        Serial.println("Pressed");

        if (lowAlarmTriggered && !lowAlarmAcknoledged) {
          lowAlarmAcknoledged = true;
          lowAlarmTriggered = false;
          highAlarmAcknoledged = false;
          setLED(LED_BLUE, OFF);
          Serial.println(F("Low alarm ack"));
          digitalWrite(BUZZ_PIN, OFF);
          blinkDisplay(false);
          deviceMode = MODE_IDLE;
          updateUnits();
        }

        else if (highAlarmTriggered && !highAlarmAcknoledged) {
          highAlarmAcknoledged = true;
          highAlarmTriggered = false;
          lowAlarmAcknoledged = false;
          setLED(LED_RED, OFF);
          setLED(LED_BLUE, ON);
          Serial.println(F("High alarm ack"));
          digitalWrite(BUZZ_PIN, OFF);
          blinkDisplay(false);
        } else {
          if (deviceMode == MODE_IDLE) {
            deviceMode = MODE_RUN;
            digitalWrite(LED_RED, HIGH);
          } else if (deviceMode == MODE_RUN) {
            deviceMode = MODE_IDLE;
            updateUnits();
          }
        }
      }
    }
    static bool lastIdleOrRun = deviceMode;
    if (bitRead(key_state, KEY1)) { // Encoder pushbutton

      Serial.print(F("Key 1 - "));
      if (bitRead(currentKeyState, KEY1)) {
        Serial.println("Released");
      } else {
        Serial.println("Pressed");

        if (deviceMode == MODE_RUN) {
          lastIdleOrRun = deviceMode;
          deviceMode++;
        }

        else if (deviceMode == MODE_IDLE) {
          lastIdleOrRun = deviceMode;
          deviceMode = MODE_USET;
        } else {
          deviceMode++;
        }

        // deviceMode++;
        if (deviceMode > MODE_LAST) {
          deviceMode = lastIdleOrRun;
        }
        setDisplayMode();
        Serial.print(F("Mode change to: "));
        Serial.println(lastIdleOrRun);
      }
    }

    // all keys have been checked, reset key_state
    key_state = 0;
  }
}

// Read the encoder pins (called from ISR vector)
inline uint8_t readEncoder(void) {

  uint8_t encoderPos = 0;

  if (!bit_is_clear(ENC_PIN, ENC_A))
    encoderPos |= (1 << 1);

  if (!bit_is_clear(ENC_PIN, ENC_B))
    encoderPos |= (1 << 0);

  return encoderPos;
}

void handleEncoder() {
  if (currentEncoderPos != lastEncoderPos) {

    if ((currentEncoderPos == 3 && lastEncoderPos == 1) ||
        (currentEncoderPos == 0 && lastEncoderPos == 2)) {
      Serial.println(F("1 +"));

      if (deviceMode == MODE_USET) {
        temperatureUnit = UNIT_C;
        updateUnits();
      } else if (deviceMode == MODE_LSET) {
        if (temperatureUnit == UNIT_C) {
          lowAlarmValue++;
        } else {
          lowAlarmValue = lowAlarmValue + 0.555555556f;
        }
        setLowAlarm(lowAlarmValue);
        Serial.print("LSET: ");
        Serial.println(lowAlarmValue);
        updateDisplay();
      } else if (deviceMode == MODE_HSET) {
        if (temperatureUnit == UNIT_C) {
          highAlarmValue++;
        } else {
          highAlarmValue = highAlarmValue + 0.555555556f;
        }
        setHighAlarm(highAlarmValue);
        updateDisplay();
      }

    } else if ((currentEncoderPos == 2 && lastEncoderPos == 0) ||
               (currentEncoderPos == 1 && lastEncoderPos == 3)) {
      Serial.println(F("1 -"));

      if (deviceMode == MODE_USET) {
        temperatureUnit = UNIT_F;
        updateUnits();
      } else if (deviceMode == MODE_LSET) {
        if (temperatureUnit == UNIT_C) {
          lowAlarmValue--;
        } else {
          lowAlarmValue = lowAlarmValue - 0.555555556f;
        }
        Serial.print("LSET: ");
        Serial.println(lowAlarmValue);
        setLowAlarm(lowAlarmValue);
        updateDisplay();
      } else if (deviceMode == MODE_HSET) {
        if (temperatureUnit == UNIT_C) {
          highAlarmValue--;
        } else {
          highAlarmValue = highAlarmValue - 0.555555556f;
        }
        setHighAlarm(highAlarmValue);
        updateDisplay();
      }
    }

    lastEncoderPos = currentEncoderPos;
  }
}

void handleInputs() {
  handleKeys();
  handleEncoder();
}

ISR(TIMER2_COMPA_vect) {
  readKeys();
  currentEncoderPos = readEncoder();
}
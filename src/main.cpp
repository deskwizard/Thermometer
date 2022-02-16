#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>


#define UNIT_C true
#define UNIT_F false
// OneWire Bus pin (requires 4.7K external pullup)
#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

bool temperatureUnit = UNIT_F; // false = F, true = C

// 7 segment display pins
uint8_t seg_array[7] = {6, 7, 8, 9, 10, 11, 12};
uint8_t com_array[4] = {2, 3, 4, 5};
uint8_t displayBuffer[4] = {17, 17, 17, 17};
uint8_t digitCount = 4;

bool num_array[][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 0, 0, 1, 1}, // 9
    {1, 1, 1, 0, 1, 1, 1}, // A
    {0, 0, 1, 1, 1, 1, 1}, // B
    {1, 0, 0, 1, 1, 1, 0}, // C
    {0, 1, 1, 1, 1, 0, 1}, // D
    {1, 0, 0, 1, 1, 1, 1}, // E
    {1, 0, 0, 0, 1, 1, 1}, // F
    {0, 0, 0, 0, 0, 0, 0}, // Blank
    {0, 0, 0, 0, 0, 0, 1}  // -
};

void timer_init() {

  // TIMER 2 for interrupt frequency 1000 Hz:
  cli(); // stop interrupts

  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2 = 0;  // initialize counter value to 0

  // set compare match register for ??? Hz increments
  OCR2A = 255; // = 8000000 / (32 * 1000) - 1 (must be <256)

  // turn on CTC mode
  TCCR2B |= (1 << WGM21);

  // Set CS22, CS21 and CS20 bits for 1024 prescaler
  TCCR2B |= (1 << CS22) | (1 << CS21) | (0 << CS20);

  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  sei(); // allow interrupts
}

void setDigit(uint8_t digit, uint8_t number) { displayBuffer[digit] = number; }

void setDisplay(uint16_t value) {
/*
  Serial.print("Value: ");
  Serial.println(value);
*/
  uint16_t hundreds = value / 100;
  uint16_t tens = (value / 10) % 10;
  uint16_t ones = value % 10;

  if (hundreds == 0) {
    displayBuffer[0] = 16;
  } else {
    displayBuffer[0] = hundreds;
  }

  if (hundreds == 0 && tens == 0) {
    displayBuffer[1] = 16;
  } else {
    displayBuffer[1] = tens;
  }

  displayBuffer[2] = ones;

/*
  Serial.print("Hund: ");
  Serial.println(hundreds);
  Serial.print("Tens: ");
  Serial.println(tens);
  Serial.print("Ones: ");
  Serial.println(ones);
  Serial.println();
  */
}

void updateDisplay() {
  static uint8_t bufferIndex = 0;

  for (uint8_t x = 0; x < 4; x++) {
    if (x != bufferIndex) {
      digitalWrite(com_array[x], HIGH);
    } else {
      digitalWrite(com_array[x], LOW);
    }
  }

  if (bufferIndex == 3) {
    uint8_t unitDigit;
    if (temperatureUnit == UNIT_F) {
      unitDigit = 0xF;
    } else {
      unitDigit = 0xC;
    }
    for (uint8_t i = 0; i < 7; i++) {
      digitalWrite(seg_array[i], num_array[unitDigit][i]);
    }
  } else {
    for (uint8_t i = 0; i < 7; i++) {
      digitalWrite(seg_array[i], num_array[displayBuffer[bufferIndex]][i]);
    }
  }

  if (bufferIndex < digitCount - 1) {
    bufferIndex++;
  } else {
    bufferIndex = 0;
  }
}

void loop() {

  static uint32_t lastSensorRead = millis();
  if (millis() - lastSensorRead > 1000) {

    sensors.requestTemperatures();

    if (temperatureUnit == UNIT_F) {
      float tempF = sensors.getTempFByIndex(0);

      // Check if reading was successful
      if (tempF != DEVICE_DISCONNECTED_F) {
        setDisplay(tempF);
      }
      // Otherwise we have a problem
      else {
        Serial.println("Error: Could not read temperature data");
      }
    }
    // Else we are in Celcius mode
    else {
      float tempC = sensors.getTempCByIndex(0);
      if (tempC != DEVICE_DISCONNECTED_C) {
        setDisplay(tempC);
      }

      // Otherwise we have a problem
      else {
        Serial.println("Error: Could not read temperature data");
      }
    }

    lastSensorRead = millis();
  }
}

void setup() {

  Serial.begin(57600);

  // Set Common pins
  for (uint8_t x = 0; x < 4; x++) {
    pinMode(com_array[x], OUTPUT);
    digitalWrite(com_array[x], HIGH);
  }

  // Set segments pins
  for (uint8_t x = 0; x < 7; x++) {
    pinMode(seg_array[x], OUTPUT);
  }

  timer_init();

  sensors.begin();

  delay(1000);

  Serial.println("Hello");

  // setDigit(2, 0xF);
  // setDisplay(284);
}

ISR(TIMER2_COMPA_vect) { updateDisplay(); }
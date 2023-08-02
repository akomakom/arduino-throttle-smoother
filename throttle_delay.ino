/**
 * Akom's smooth throttle using an Arduino (Pro Micro 5V or similar)
 * 
 * Takes care of:
 * - Eliminating jerky throttle response
 * - Eliminating deadband
 * - Speed Limit (adjustable by potentiometer)
 */

// Input pin for throttle signal
#define PIN_IN A1
// Output pin to the controller
#define PIN_OUT 10
// Input pin for the speed limiting potentiometer
#define PIN_LIMIT A3
// ms delay between debugging prints
#define PRINT_DELAY 100

/* fine tune the throttle range to eliminate deadband */
// Normal range of throttle
#define THROTTLE_MAP_IN_MIN 180
#define THROTTLE_MAP_IN_MAX 850
// Range we want to send to the controller
// values from "no movement yet" to "max speed":
#define THROTTLE_MAP_OUT_MIN 390
#define THROTTLE_MAP_OUT_MAX 800

/* fine tune the speed limit potentiometer range */
// input from potentiometer - possible values on analogRead, adjust to your pot's range
#define LIMIT_MAP_IN_MIN 0
#define LIMIT_MAP_IN_MAX 1023
// this adjusts throttle output speed limit (min on pot to max on pot)
// "about as slow as it can move" to max speed - change the number to your needs
#define LIMIT_MAP_OUT_MIN THROTTLE_MAP_IN_MIN + 100
#define LIMIT_MAP_OUT_MAX THROTTLE_MAP_IN_MAX

// how quickly to adjust output, larger values are slower
#define INCREASE_ERROR_FACTOR 1400
#define DECREASE_ERROR_FACTOR 100

#define TICK_LENGTH_MS 1

// operational variables

int throttleValue = 0;      //input value from 3-wire throttle 
int limitValue = 0;         //input value from potentiometer 
int delta = 0;              //computed error from last setting (outputValue)
float outputValue = 0;      // 0-1024, later mapped to 0-255
float adjustmentAmount = 0; // 
int mapped = 0;             // outputValue after mapping to controller range
unsigned long lastPrint = 0;


void setup() {
  Serial.begin(115200);
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_LIMIT, INPUT);
  pinMode(PIN_OUT, OUTPUT);
  outputValue = analogRead(PIN_IN); // initial value
}

void loop() {
  throttleValue = analogRead(PIN_IN);
  limitValue = analogRead(PIN_LIMIT);
  delta = throttleValue - outputValue; // error
  adjustmentAmount = (float)delta / (float)(delta > 0 ? INCREASE_ERROR_FACTOR : DECREASE_ERROR_FACTOR);

  // Apply speed limit - allow increase only if below limit
  if (adjustmentAmount < 0 || 
      outputValue < map(limitValue, LIMIT_MAP_IN_MIN, LIMIT_MAP_IN_MAX, LIMIT_MAP_OUT_MIN, LIMIT_MAP_OUT_MAX)) {
    outputValue += adjustmentAmount;
  }

  // throttle to output value map
  mapped = map(
      outputValue, 
      THROTTLE_MAP_IN_MIN, 
      THROTTLE_MAP_IN_MAX, 
      THROTTLE_MAP_OUT_MIN, 
      THROTTLE_MAP_OUT_MAX
    );

  analogWrite(
    PIN_OUT, 
    mapped / 4
  );

  if ((lastPrint + PRINT_DELAY) < millis()) {
    lastPrint = millis();
    Serial.print("Input: ");
    Serial.print(throttleValue);
    Serial.print(" Output: ");
    Serial.print(outputValue);
    Serial.print(" Mapped: ");
    Serial.print(mapped);
    Serial.print(" +/-: ");
    Serial.print(adjustmentAmount);
    Serial.print(" Lim: ");
    Serial.print(limitValue);
    Serial.print(" Lim Map: " );
    Serial.print(map(limitValue, LIMIT_MAP_IN_MIN, LIMIT_MAP_IN_MAX, LIMIT_MAP_OUT_MIN, LIMIT_MAP_OUT_MAX));
    Serial.println("");
  }

  delay(TICK_LENGTH_MS);
  
}

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

/**
 * All the ranges below can be determined by watching the serial console and twisting the throttle
 * Note that they will be slightly wrong if the controller supplies less than 5v to throttle.
 */

/* fine tune the throttle range to eliminate deadband */
// Normal range of throttle
#define THROTTLE_MAP_IN_MIN 180
#define THROTTLE_MAP_IN_MAX 850
// Range we want to send to the controller
// values from "no movement yet" to "max speed":
#define THROTTLE_MAP_OUT_MIN 390
#define THROTTLE_MAP_OUT_MAX 800

/* Speed Limit */
// comment out this line to disable speed limit feature
#define LIMIT_ENABLE
/* fine tune the speed limit potentiometer range */
// input from potentiometer - possible values on analogRead, adjust to your pot's range
// (min from pot to max from pot).   
// Defaults are appropriate for a linear 10K ohm pot like a LA42DWQ-22
#define LIMIT_MAP_IN_MIN 0
#define LIMIT_MAP_IN_MAX 1023

// this adjusts throttle output speed limit range.
// value is applied to the throttle input range (THROTTLE_MAP_IN_*)
// Adjust to "about as slow as is practical" to "max speed" - change the added number to your needs
// you can also subtract from max to disallow full speed (hardcoded speed limit)
// like this:  #define LIMIT_MAP_OUT_MAX THROTTLE_MAP_IN_MAX - 300
// For example, if your throttle range is 200-800, then an output limit range of 300-800 is reasonable.
// This means that when your limit knob is all the way down, throttle range will be restricted to 200-300.
#define LIMIT_MAP_OUT_MIN THROTTLE_MAP_IN_MIN + 100
#define LIMIT_MAP_OUT_MAX THROTTLE_MAP_IN_MAX

/* Jerkiness Mitigation */
// how quickly to adjust output, larger values are slower
#define INCREASE_ERROR_FACTOR 170
#define DECREASE_ERROR_FACTOR 20

// Basically delay between loops:
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
    delta = throttleValue - outputValue; // error
    adjustmentAmount = (float) delta / (float) (delta > 0 ? INCREASE_ERROR_FACTOR : DECREASE_ERROR_FACTOR);

#ifdef LIMIT_ENABLE
    limitValue = analogRead(PIN_LIMIT);
    // Apply speed limit - allow increase only if below limit
    if (outputValue > map(limitValue, LIMIT_MAP_IN_MIN, LIMIT_MAP_IN_MAX, LIMIT_MAP_OUT_MIN, LIMIT_MAP_OUT_MAX)) {
        adjustmentAmount = min(adjustmentAmount, 0); // always allow decrease
    }
#endif
    outputValue += adjustmentAmount;

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
            mapped / 4 // PWM is 0-254 while our values are 0-1023
    );

    if (Serial && (lastPrint + PRINT_DELAY) < millis()) {
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
        Serial.print(" Lim Map: ");
        Serial.print(map(limitValue, LIMIT_MAP_IN_MIN, LIMIT_MAP_IN_MAX, LIMIT_MAP_OUT_MIN, LIMIT_MAP_OUT_MAX));
        Serial.println("");
    }

    delay(TICK_LENGTH_MS);

}

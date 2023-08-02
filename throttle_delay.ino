#define PIN_IN A1
#define PIN_OUT 10
#define PRINT_DELAY 100

// fine tune the throttle range to eliminate deadband
#define THROTTLE_MAP_IN_MIN 180
#define THROTTLE_MAP_IN_MAX 850
#define THROTTLE_MAP_OUT_MIN 390
#define THROTTLE_MAP_OUT_MAX 800

// how quickly to adjust output, larger values are slower
#define INCREASE_ERROR_FACTOR 1200
#define DECREASE_ERROR_FACTOR 300

#define TICK_LENGTH_MS 1


int throttleValue = 0; //input value (0-1024)
int delta = 0;
float adjustmentAmount = 0;

float outputValue = 0; // 0-1024, later mapped to 0-255
unsigned long lastPrint = 0;



void setup() {
  Serial.begin(115200);
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_OUT, OUTPUT);
  outputValue = analogRead(PIN_IN); // initial value
}

void loop() {
  throttleValue = analogRead(PIN_IN);
  delta = throttleValue - outputValue; // error
  adjustmentAmount = (float)delta / (float)(delta > 0 ? INCREASE_ERROR_FACTOR : DECREASE_ERROR_FACTOR);
  outputValue += adjustmentAmount;
    
  analogWrite(
    PIN_OUT, 
    map(
      outputValue, 
      THROTTLE_MAP_IN_MIN, 
      THROTTLE_MAP_IN_MAX, 
      THROTTLE_MAP_OUT_MIN, 
      THROTTLE_MAP_OUT_MAX
    ) / 4
  );

  if ((lastPrint + PRINT_DELAY) < millis()) {
    lastPrint = millis();
    Serial.print("Input: ");
    Serial.print(throttleValue);
    Serial.print(" Output: ");
    Serial.print(outputValue);
    Serial.print(" Adj: ");
    Serial.print(adjustmentAmount);
    Serial.println("");
  }

  delay(TICK_LENGTH_MS);
  
}

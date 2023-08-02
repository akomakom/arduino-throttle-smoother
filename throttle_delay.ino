#define PIN_IN A1
#define PIN_OUT 10
#define PRINT_DELAY 100

// How often and by how much to increase throttle value when going up
#define INCREASE_STEP 1
#define INCREASE_DELAY 10
#define DECREASE_STEP 2
#define DECREASE_DELAY 2

/** JITTER MITIGATION **/
// variations in throttle below this amount are adjusted more slowly
#define JITTER_THRESHOLD 100
// adjustment rate
#define JITTER_STEP 1
#define JITTER_DELAY 100
// jitter logic is only effective between these values
// at low (starting) throttle, jitter can result in long creep times
#define JITTER_MIN 450
#define JITTER_MAX 900

// TODO
#define THROTTLE_MIN 187
#define THROTTLE_MAX 865

int throttleValue = 0; //input value (0-1024)
int outputValue = 0; // 0-1024, later mapped to 0-255
unsigned long lastPrint = 0;
unsigned long lastIncrease = 0;
unsigned long lastDecrease = 0;
unsigned long lastJitterAdjust = 0;



void setup() {
  Serial.begin(115200);
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_OUT, OUTPUT);
  outputValue = analogRead(PIN_IN); // initial value
}

void adjust(
    int inValue, 
    int * outputValue, 
    unsigned long *lastAdjustIncrease, 
    unsigned long *lastAdjustDecrease, 
    int increaseStep, 
    int decreaseStep, 
    int increaseDelay, 
    int decreaseDelay) {

  int delta = inValue - *outputValue;
  int step = (delta > 0) ? increaseStep : decreaseStep;
  int del = (delta > 0) ? increaseDelay : decreaseDelay;
  unsigned long *lastAdjust = (delta > 0) ? lastAdjustIncrease : lastAdjustDecrease;
  
  if ((*lastAdjust + del) < millis() && delta != 0) {
    *lastAdjust = millis();
    *outputValue += (delta > 0) ? min(step, delta) : max(-step, delta);
  }

}

void loop() {
  throttleValue = analogRead(PIN_IN);

  int delta = throttleValue - outputValue;
  if (abs(delta) < JITTER_THRESHOLD && throttleValue > JITTER_MIN && throttleValue < JITTER_MAX) {
    adjust(throttleValue, &outputValue, &lastJitterAdjust, &lastJitterAdjust, JITTER_STEP, JITTER_STEP, JITTER_DELAY, JITTER_DELAY);
  } else {
    adjust(throttleValue, &outputValue, &lastIncrease, &lastDecrease, INCREASE_STEP, DECREASE_STEP, INCREASE_DELAY, DECREASE_DELAY);
  }
  
    
  analogWrite(PIN_OUT, map(outputValue, 180, 850, 390, 800) / 4);
//  analogWrite(PIN_OUT, outputValue / 4);
//  analogWrite(PIN_OUT, 127);

  if ((lastPrint + PRINT_DELAY) < millis()) {
    lastPrint = millis();
    Serial.print("Input: ");
    Serial.print(throttleValue);
    Serial.print(" Output: ");
    Serial.print(outputValue);
    Serial.println("");
  }

  
}

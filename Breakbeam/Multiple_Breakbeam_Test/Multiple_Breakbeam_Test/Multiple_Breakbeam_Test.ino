/*
Simple test for a multiple breakbeam sensors
Ouputs 1(no obstruction) or 0(obstruction) for anything in front of sensor

Kevin Xu
*/

#define ARRAY_SIZE(array) ((sizeof(array)) / (sizeof(array[0])))

// numbers correspond with pins on board
const int breakbeamSensor[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const int numSensors = ARRAY_SIZE(breakbeamSensor);

void setup() {
  Serial.begin(9600);

  // intialize all sensors
  for (int ii = 0; ii < numSensors - 1; ii++) {
    pinMode(breakbeamSensor[ii], INPUT);
  }
}

void loop() {
  int state[20] = { 0 };

  // set up all states of sensors (either 1 or 0)
  for (int ii = 0; ii < numSensors - 1; ii++) {
    state[ii] = digitalRead(breakbeamSensor[ii]);
  }

  // outputs states
  for (int ii = 0; ii < numSensors - 1; ii++) {
    Serial.print(state[ii]);
    Serial.print("   ");
  }

  Serial.println();

  // delay between reads for stability
  delay(1);
}
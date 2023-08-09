/*
Simple test for a singular breakbeam sensor
Ouputs 1(no obstruction) or 0(obstruction) in front of sensor

Kevin Xu
*/

const int breakbeamSensor = 7;

void setup() {
  Serial.begin(9600);
  pinMode(breakbeamSensor, INPUT);
}

void loop() {
  // state of sensor (either 1 or 0)
  int state = digitalRead(breakbeamSensor);
  
  // outputs state
  Serial.println(state);
  
  if (state == LOW) {
    Serial.println("Object Detected");
  } 
  else {
    Serial.println("All Clear");
  }

  // delay between reads for stability
  delay(1);
}
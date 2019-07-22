#include"autonomous.h"

CarAI AI;

void setup() {

  AI.SetupAI();
  
  Serial.begin(9600);
  
  delay(5000);
}

void loop() {
  AI.Run();
}

/**
* USED TO TEST THE PI CONTROLLER!
* By Franco Heraud
*/
 
 
/*
 
#include "Motor.h"
 
Motor_t motor;
 
void setup() {
  Serial.begin(115200);
  initMotors();
  Serial.println("set up complete");
}
 
void loop() {
  //updatePIController(&motor, )
  motor.direction = FORWARD;
  move(&motor);
 
  static unsigned long lastTime = millis();
  static int encoderCountOldA = 0, encoderCountOldB = 0;
  
  if (millis() - lastTime > 500) {
    // x100 added due to velocity being a bit low
    float velA = 100.0f * (encoderCountA - encoderCountOldA) / (float)PI_UPDATE_INTERVAL_MS;
    float velB = 100.0f * (encoderCountB - encoderCountOldB) / (float)PI_UPDATE_INTERVAL_MS;
 
    //Serial.printf("---------------------\n");
    Serial.printf("EncA: %d | velA: %.2f | rmot: %d \n", encoderCountA, velA, rMotNewA);
    
    updatePIController(&motor, velA, velB);
    
    encoderCountOldA = encoderCountA;
    encoderCountOldB = encoderCountB;
    lastTime = millis();
  }
}
  */
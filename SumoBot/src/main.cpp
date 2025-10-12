/**
* USED TO TEST THE PI CONTROLLER!
* By Franco Heraud
*/
 
 
 
#include "Motor.h"
#include "StartupMenu.h"
 
Motor_t motor;
TFT_eSPI tft = TFT_eSPI();
 
void setup() {
  Serial.begin(115200);
  initMotors();
  Serial.println("set up complete");
  tft.init();
  tft.println("Setup Complete!"); 
}
 

static void updateController(void) {
  static unsigned long lastTime = millis();
  static int encoderCountOldA = 0, encoderCountOldB = 0;
  
  if (millis() - lastTime > 500) {
    // x100 added due to velocity being a bit low
    float velA = 100.0f * (encoderCountA - encoderCountOldA) / (float)PI_UPDATE_INTERVAL_MS;
    float velB = 100.0f * (encoderCountB - encoderCountOldB) / (float)PI_UPDATE_INTERVAL_MS;

    //Serial.printf("---------------------\n");
    //Serial.printf("EncA: %d | velA: %.2f | rmot: %d \n", encoderCountA, velA, rMotNewA);
    Serial.printf("%.2f, %.2f\n", motor.desiredSpeedA, velA);

    updatePIController(&motor, velA, velB);
    
    encoderCountOldA = encoderCountA;
    encoderCountOldB = encoderCountB;
    lastTime = millis();
  }
}

// integrating the PI Controller and some demo functions for moving the sumobot
// forward -> reverse -> left -> right -> CW -> CCW
void loop() {
  static int demoState = 0;
  static unsigned long last = millis();

  updateController();  // continuously update PI loop

  if (millis() - last > 3000) {
    stopMotors();
    delay(500);  // small pause between moves

    demoState++;
    if (demoState > 5)
      demoState = 0;  // loop back to start

    motor.direction = (Direction)demoState;
    move(&motor);
    last = millis();
  }
}

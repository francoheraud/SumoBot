#include "StartupMenu.h"
#include "UltrasonicDistance.h"
#include "LineDetector.h"
#include "Motor.h"

TFT_eSPI tft = TFT_eSPI();


void setup() {
  tft.init();
  setupUltrasonicSensor(true);
}

void loop() {
  pollUltrasonicSensor();
}
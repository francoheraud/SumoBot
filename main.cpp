// MAIN CODE
// WIP

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "StartupMenu.h"
#include "Ultrasonic.h"
#include "LineDetector.h"
#include "MotorDriver.h"

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
}

void loop() {
  ; // does nothing
}
// MAIN CODE
// WIP

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "startup.h"
#include "ultrasonic.h"
#include "linedetector.h"
#include "motordriver.h"

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
}

void loop() {
  ; // does nothing
}

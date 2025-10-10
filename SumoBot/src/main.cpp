#include <Arduino.h>
#include <TFT_eSPI.h>
#include "StartupMenu.h"
#include "Sensors.h"
#include "Motor.h"

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
}

void loop() {
  delay(100);
}
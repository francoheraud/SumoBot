#ifndef STARTUP_H
#define STARTUP_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#define MENU_X_DATUM    32
#define MENU_Y_DATUM    20
#define BUFFER_CHARS    50

#define LEFT_BUTTON     0
#define RIGHT_BUTTON    0

#define PRIMARY_TEXT_COLOUR     TFT_WHITE
#define HIGH_EMPHASIS_COLOUR    TFT_GOLD
#define LOW_EMPHASIS_COLOUR     0x2965
#define BACKGROUND_COLOUR       TFT_BLACK

enum robotMode {
  DEBUG_MODE,
  TEST_1_DRIVE_STRAIGHT,
  TEST_2_ROTATION,
  TEST_3_TBD,
  TEST_4_TBD,
  COMPETITION_MODE
};





void userSelectFunction(TFT_eSPI *tft);



#endif
// Startup code (general user UI)
// Allan Wu (23810308)
// 21 September 2025
#include <Arduino.h>
#include <TFT_eSPI.h>

// This is driver code (can run as main.cpp), I still need to make the header file and write a setup function
#define MENU_X_DATUM 32
#define MENU_Y_DATUM 20
#define BUFFER_CHARS 50

const int LEFT_BUTTON = 0;
const int RIGHT_BUTTON = 14;

uint16_t PRIMARY_TEXT_COLOUR = TFT_WHITE;
uint16_t HIGH_EMPHASIS_COLOUR = TFT_GOLD;
uint16_t LOW_EMPHASIS_COLOUR = 0x2965;
uint16_t BACKGROUND_COLOUR = TFT_BLACK;

enum robotMode {
  DEBUG_MODE,
  TEST_1_DRIVE_STRAIGHT,
  TEST_2_ROTATION,
  TEST_3_TBD,
  TEST_4_TBD,
  COMPETITION_MODE
};
char robotModeDescriptions[6][BUFFER_CHARS] = {
  "0. DEBUG MODE",
  "1. TEST 1 (DRIVE STRAIGHT)",
  "2. TEST 2 (ROTATION)",
  "3. TEST 3 (TBD)",
  "4. TEST 4 (TBD)",
  "5. COMPETITION_MODE"
};
robotMode modeOfOperation = DEBUG_MODE;

TFT_eSPI tft = TFT_eSPI();

void userSelectFunction() {
  int prevLeft = 0, prevRight = 0;
  int currLeft = 0, currRight = 0;
  int prevChoice = -1, currChoice = 0;
  bool startOperation = false;
  unsigned long lastUpdateTime = millis();
  float delaySeconds = 9.9;
  char titleText[BUFFER_CHARS];
  tft.setTextFont(2);
  while (!startOperation) {
    float countdownSeconds = delaySeconds - (millis() - lastUpdateTime)/1000.0;
    snprintf(titleText, BUFFER_CHARS, "SELECT MODE (START IN %.1f s)   ", countdownSeconds);
    tft.drawString(titleText, MENU_X_DATUM, MENU_Y_DATUM-5);
    currLeft = !digitalRead(LEFT_BUTTON);
    currRight = !digitalRead(RIGHT_BUTTON);
    if (prevRight && !currRight) {
      currChoice = (currChoice + 1) % 6; // Must update this whenever we add/remove robotModes
      lastUpdateTime = millis();
    } else if (prevLeft && !currLeft) {
      currChoice = (currChoice + 5) % 6;
      lastUpdateTime = millis();
    }
    if (prevChoice != currChoice) {
      tft.setTextColor(LOW_EMPHASIS_COLOUR, BACKGROUND_COLOUR);
      for (int i = 0; i < 6; i++) {
        tft.drawString(robotModeDescriptions[i], MENU_X_DATUM, MENU_Y_DATUM+20*(i+1));
      }
      tft.setTextColor(HIGH_EMPHASIS_COLOUR, BACKGROUND_COLOUR);
      tft.drawString(robotModeDescriptions[currChoice], MENU_X_DATUM, MENU_Y_DATUM+20*(currChoice+1));
      switch (currChoice) {
        case (0):
          modeOfOperation = DEBUG_MODE;
          break;
        case (1):
          modeOfOperation = TEST_1_DRIVE_STRAIGHT;
          break;
        case (2):
          modeOfOperation = TEST_2_ROTATION;
          break;
        case (3):
          modeOfOperation = TEST_3_TBD;
          break;
        case (4):
          modeOfOperation = TEST_4_TBD;
          break;
        case (5):
          modeOfOperation = COMPETITION_MODE;
          break;
      }
    }
    tft.setTextColor(PRIMARY_TEXT_COLOUR, BACKGROUND_COLOUR);
    prevChoice = currChoice;
    if (countdownSeconds <= 0) {
      startOperation = true;
    }
    prevLeft = currLeft;
    prevRight = currRight;
  }
}

void setup() {
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);
  userSelectFunction();
}

void loop() {
  tft.fillScreen(HIGH_EMPHASIS_COLOUR);
  delay(1000);
}

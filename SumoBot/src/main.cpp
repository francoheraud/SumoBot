#include "Motor.h"
#include "Sensors.h"
#include "Startup.h"

TFT_eSPI tft = TFT_eSPI();
Sensors_t sensor;
Motor_t motor;

#define BUF_SIZE                 8
#define DETECTION_THRESHOLD      30
#define TRACK_OPPONENT_THRESHOLD 20
#define LOST_REQUIRED            6
#define EDGE_AVOID_TIME_MS       4000
#define STARTUP_ROTATE_DELAY_MS  200   // delay between rotation checks during startup

int distanceBuf[BUF_SIZE] = {0};
int bufIdx = 0;
bool bufferFilled = false;

enum RobotState { STARTUP_ROTATE, SEARCHING, CHASING, AVOID_EDGE };
RobotState currentState = STARTUP_ROTATE;
RobotState prevState = CHASING;

Direction lastSeenDirection = ROTATE_CCW;
unsigned long lastPIUpdate = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long edgeAvoidStart = 0;
static int detectConfirmCount = 0;

void setup() {
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);

  initMotors();
  initSensors();

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  userSelectFunction(&tft, &sensor, &motor);

  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  for (int i = 0; i < BUF_SIZE; ++i) distanceBuf[i] = 1000;
  bufIdx = 0;
  bufferFilled = false;
  lastPIUpdate = millis();

  currentState = STARTUP_ROTATE;
  motor.direction = ROTATE_CW;
  move(&motor);
}

static int getAverageDistance() {
  long sum = 0;
  for (int i = 0; i < BUF_SIZE; i++) sum += distanceBuf[i];
  return (int)(sum / BUF_SIZE);
}

static void updateDistanceBuf(int distance) {
  distanceBuf[bufIdx] = distance;
  bufIdx = (bufIdx + 1) % BUF_SIZE;
  if (bufIdx == 0) bufferFilled = true;
}

static bool detectOpponent() {
  if (!bufferFilled) return false;
  int avg = getAverageDistance();

  int currIdx = (bufIdx - 1 + BUF_SIZE) % BUF_SIZE;
  int curr = distanceBuf[currIdx];

  bool dropDetected = (avg - curr) > DETECTION_THRESHOLD || curr < TRACK_OPPONENT_THRESHOLD;
  if (dropDetected) detectConfirmCount++;

  else detectConfirmCount = max(0, detectConfirmCount - 1);
  return (detectConfirmCount >= 2);
}

static void updateMotorControl() {
  static int encoderCountOldA = 0, encoderCountOldB = 0;
  unsigned long now = millis();
  unsigned long elapsedMs = now - lastPIUpdate;
  if (elapsedMs < PI_UPDATE_INTERVAL_MS) return;
  if (elapsedMs == 0) elapsedMs = 1;

  float velA = 100.0f * (encoderCountA - encoderCountOldA) / (float)elapsedMs;
  float velB = 100.0f * (encoderCountB - encoderCountOldB) / (float)elapsedMs;
  updatePIController(&motor, velA, velB);
  encoderCountOldA = encoderCountA;
  encoderCountOldB = encoderCountB;
  lastPIUpdate = now;
}

static void chaseMode() {
  bool haveBoth = (sensor.leftCm != OUT_OF_RANGE && sensor.rightCm != OUT_OF_RANGE);
  bool opponentLeft = haveBoth && (sensor.leftCm < sensor.rightCm - TRACK_OPPONENT_THRESHOLD);
  bool opponentRight = haveBoth && (sensor.rightCm < sensor.leftCm - TRACK_OPPONENT_THRESHOLD);

  if (opponentLeft) {
    motor.direction = LEFT;
    lastSeenDirection = LEFT;
  } else if (opponentRight) {
    motor.direction = RIGHT;
    lastSeenDirection = RIGHT;
  } else {
    motor.direction = FORWARD;
  }

  move(&motor);
}

static bool lineDetected() {
  return (sensor.frontLeft || sensor.frontRight || sensor.rearLeft || sensor.rearRight);
}

static Direction getNextDirection() {
  if (sensor.frontLeft || sensor.frontRight)  return REVERSE;
  else if (sensor.rearLeft && sensor.rearRight)   return FORWARD;
  //else if (sensor.frontLeft)   return ROTATE_CCW;
  //else if (sensor.frontRight)  return ROTATE_CW;
  else if (sensor.rearLeft)   return RIGHT;
  else if (sensor.rearRight)  return LEFT;
}

/*
  if (sensor.frontLeft && sensor.frontRight)  return REVERSE;
  else if (sensor.rearLeft && sensor.rearRight)  return FORWARD;
  else if (sensor.frontLeft)  return RIGHT;
  else if (sensor.frontRight) return LEFT;
  else if (sensor.rearLeft)   return RIGHT;
  else if (sensor.rearRight)  return LEFT;
  return ROTATE_CW;
*/

static void updateDisplay(int left, int right, int avg) {
  unsigned long now = millis();
  if (now - lastDisplayUpdate < 100) return;
  static int statusColor = TFT_BLACK;

  switch (currentState) {
    case STARTUP_ROTATE:
      statusColor = TFT_CYAN;
      break;
    case SEARCHING:
      statusColor = TFT_GOLD;
      break;
    case CHASING:
      statusColor = TFT_GREEN;
      break;
    case AVOID_EDGE:
    default:
      statusColor = TFT_MAROON;
      break;
  }
  if (currentState != prevState) {
    tft.fillScreen(statusColor);
    tft.setTextColor(TFT_BLACK, statusColor);
  }

  tft.setCursor(0, 0);
  tft.printf("Left :%4d cm\n", left);
  tft.printf("Right:%4d cm\n", right);
  tft.printf("Avg  :%4d cm\n", avg);
  tft.printf("State: %8s\n",
    (currentState == STARTUP_ROTATE) ? "STARTUP" :
    (currentState == SEARCHING) ? "SEARCH" :
    (currentState == CHASING)  ? "CHASE" : "EDGE");
  tft.printf("FL:%d FR:%d \nRL:%d RR:%d\n%4d",
    sensor.frontLeft, sensor.frontRight, sensor.rearLeft, sensor.rearRight, sensor.analogReading);

  prevState = currentState;
}

void loop() {
  pollDistance(&sensor);
  delay(5);
  pollDistance(&sensor);
  detectLine(&sensor);

  int left = sensor.leftCm;
  int right = sensor.rightCm;
  int avg = (left + right) / 2;
  updateDistanceBuf(avg);

  switch (currentState) {
    case STARTUP_ROTATE:
      motor.direction = ROTATE_CW;
      move(&motor);
      if (detectOpponent()) {
        currentState = CHASING;
        detectConfirmCount = 0;
      }
      delay(STARTUP_ROTATE_DELAY_MS);
      break;

    case SEARCHING:
      motor.direction = lastSeenDirection;
      move(&motor);
      if (lineDetected()) {
        currentState = AVOID_EDGE;
        edgeAvoidStart = millis();
        move(&motor);
      } else if (detectOpponent()) {
        currentState = CHASING;
        detectConfirmCount = 0;
      }
      break;

    case CHASING:
      chaseMode();
      if (lineDetected()) {
        currentState = AVOID_EDGE;

        move(&motor);
      } else if (sensor.leftCm == OUT_OF_RANGE && sensor.rightCm == OUT_OF_RANGE) {
        currentState = SEARCHING;
      }
      break;

    case AVOID_EDGE:
      motor.direction = getNextDirection();
      move(&motor);
      if (millis() - edgeAvoidStart > EDGE_AVOID_TIME_MS) {
        edgeAvoidStart = millis();
        currentState = SEARCHING;
      }
      break;
  }
  updateMotorControl();
  updateDisplay(sensor.leftCm, sensor.rightCm, avg);
}
// Ultrasonic Distance Sensor Code
// Author: Allan Wu (23810308)
// Last updated: 14 September 2025 at 3 pm

/*
TODO: Calculate a moving average of distance readings (handle false OUT_OF_RANGE readings?)
- Define some kind of output struct? E.g.: double distanceCm, bool outOfRange, unsigned long elapsedTime
*/

#include <Arduino.h>
#include <TFT_eSPI.h>

const static int ultrasonicTriggerPin = 1;
const static int ultrasonicEchoPin = 2;

// Set the maximum time we will wait for the echo pulse: this determines what is "OUT OF RANGE" for the sensor!
// From my testing, 15000 us timeout limits the distane range to ~200 cm
const static int timeoutMicroseconds = 15000;

// Time to wait before running the loop again, note that maximum polling rate is 50 Hz => 20,000 us delay between reads
const static int pollDelayMicroseconds = 50000;

// Set displayOutputEnabled to 1 to enable printing to TFT, otherwise the only output is the serial monitor
int static displayOutputEnabled = 1;

// Text colour settings (for aesthetic purposes only)
const static int maximumRangeCm = 100;
const static int longRangeCm = 60;
const static int mediumRangeCm = 40;
const static int shortRangeCm = 20;

const static uint32_t outOfRangeColour = TFT_DARKGREY;
const static uint32_t maximumRangeColour = TFT_WHITE;
const static uint32_t longRangeColour = TFT_GREEN;
const static uint32_t mediumRangeColour = TFT_GOLD;
const static uint32_t shortRangeColour = TFT_RED;

TFT_eSPI tft = TFT_eSPI();

void setup() {
  // Uncomment the following two lines to turn on display power (if running on battery power)
  // pinMode(15, OUTPUT);
  // digitalWrite(15, HIGH);

  pinMode(ultrasonicTriggerPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);
  Serial.begin(9600);

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setRotation(3);
}

void loop() {
  static unsigned long durationMicroseconds;
  static double distanceCm;
  static unsigned long startTimeMicroseconds, elapsedTimeMicroseconds = 0;
  static char distancePrintout[20];

  digitalWrite(ultrasonicTriggerPin, LOW); // Write low to ensure a clean trigger pulse
  delayMicroseconds(2);
  digitalWrite(ultrasonicTriggerPin, HIGH); // Trigger pulse must be high for minimum 10 us
  delayMicroseconds(10);
  digitalWrite(ultrasonicTriggerPin, LOW);

  startTimeMicroseconds = micros();

  // Call pulseIn() to read the echo pulse duration, if timeout occurs duration is instead set to 0
  durationMicroseconds = pulseIn(ultrasonicEchoPin, HIGH, timeoutMicroseconds);

  if (durationMicroseconds > 0) {
    // Calculation: distance = v*t = (343 m/s) * (100 c/m) * (time in us) * (0.000001 s / us) / 2
    distanceCm = (durationMicroseconds * 0.0343) / 2;
    snprintf(distancePrintout, 20, "%.2f cm   ", distanceCm);
  } else {
    snprintf(distancePrintout, 20, "??? cm   ");
  }

  // Output to screen, setting text colour for easier reading
  if (displayOutputEnabled) {
    if (durationMicroseconds == 0) tft.setTextColor(outOfRangeColour, TFT_BLACK);
    else if (distanceCm < shortRangeCm) tft.setTextColor(shortRangeColour, TFT_BLACK);
    else if (distanceCm < mediumRangeCm) tft.setTextColor(mediumRangeColour, TFT_BLACK);
    else if (distanceCm < longRangeCm) tft.setTextColor(longRangeColour, TFT_BLACK);
    else if (distanceCm < maximumRangeCm) tft.setTextColor(maximumRangeColour, TFT_BLACK);
    else tft.setTextColor(outOfRangeColour, TFT_BLACK);

    tft.drawString(distancePrintout, 0, 40);
  }

  // Output to serial monitor
  elapsedTimeMicroseconds = micros() - startTimeMicroseconds;
  Serial.println(elapsedTimeMicroseconds);
  Serial.println(distancePrintout);

  // Wait before the next measurement
  if (pollDelayMicroseconds > elapsedTimeMicroseconds)
    delayMicroseconds(pollDelayMicroseconds - elapsedTimeMicroseconds);
}
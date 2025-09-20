// Ultrasonic sensor code
// Allan Wu (23810308)
// 20 September 2025

#include <Arduino.h>
#include <TFT_eSPI.h>

// Global variable that can be referenced elsewhere, note that a negative value indicates no enemy found
int distanceCmRounded = -1;

const static int ultrasonicTriggerPin = 1;
const static int ultrasonicEchoPin = 2;

// Set the maximum time we will wait for the echo pulse: this determines what is "OUT OF RANGE" for the sensor!
// From my testing, 15000 us timeout limits the distane range to ~200 cm
const static int timeoutMicroseconds = 15000;

// Time to wait before running the loop again, note that maximum polling rate is 50 Hz => 20,000 us delay between reads
const static int pollDelayMicroseconds = 50000;

// Set ultrasonicSerialEnable to 1 to enable printout to the serial monitor
int ultrasonicSerialEnable = 1;

static TFT_eSPI tft = TFT_eSPI();

void setupUltrasonicSensor() {
    pinMode(ultrasonicTriggerPin, OUTPUT);
    pinMode(ultrasonicEchoPin, INPUT);
    if (ultrasonicSerialEnable) Serial.begin(115200);
}

unsigned long pollUltrasonicSensor() {
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
        snprintf(distancePrintout, 20, "%.2f cm     ", distanceCm);
    } else {
        distanceCm = -1;
        snprintf(distancePrintout, 20, "??? cm     ");
    }

    // Save result to global variable
    distanceCmRounded = (int)(distanceCm + 0.5);

    // Output to serial monitor
    if (ultrasonicSerialEnable) {
        elapsedTimeMicroseconds = micros() - startTimeMicroseconds;
        Serial.println(elapsedTimeMicroseconds);
        Serial.println(distancePrintout);
        Serial.println(distanceCmRounded);
    }
    return elapsedTimeMicroseconds; // Returns elapsed time in microseconds
        
    // Wait before the next measurement
    // if (pollDelayMicroseconds > elapsedTimeMicroseconds)
        // delayMicroseconds(pollDelayMicroseconds - elapsedTimeMicroseconds);
}
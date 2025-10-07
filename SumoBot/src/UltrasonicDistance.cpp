// Ultrasonic sensor code
// Original Author: Allan Wu (23810308)
// Rework: Franco Heraud (23620168)
#include "UltrasonicDistance.h"

// -1 is a sentinel value meaning no enemy found
double ultrasonicDistanceCm = -1;
int ultrasonicDistanceNearestCm = -1;

// Set ultrasonicSerialEnable to 1 to enable printout to the serial monitor
bool ultrasonicSerialEnable;


// Time to wait before running the loop again, note that maximum polling rate is 50 Hz => 20,000 us delay between reads
int pollDelayMicroseconds = 50000;


void setupUltrasonicSensor(bool serialEnable) {
    pinMode(ULTRASONIC1_TRIGGER, OUTPUT);
    pinMode(ULTRASONIC1_ECHO, INPUT);
    if (serialEnable) {
        Serial.begin(115200);
        ultrasonicSerialEnable = true;
    }
}

int pollUltrasonicSensor(void) {
    static unsigned long durationMicroseconds;
    static unsigned long startTimeMicroseconds, elapsedTimeMicroseconds = 0;
    static char distancePrintout[20];

    digitalWrite(ULTRASONIC1_TRIGGER, LOW); 
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC1_TRIGGER, HIGH); 
    delayMicroseconds(10); // Trigger pulse must be high for minimum 10 us
    digitalWrite(ULTRASONIC1_TRIGGER, LOW);

    startTimeMicroseconds = micros();

    // Call pulseIn() to read the echo pulse duration, if timeout occurs duration is instead set to 0
    durationMicroseconds = pulseIn(ULTRASONIC1_ECHO, HIGH, ULTRASONIC_TIMEOUT_US);

    if (durationMicroseconds > 0) {
        // Calculation: distance = v*t = (343 m/s) * (100 c/m) * (time in us) * (0.000001 s / us) / 2
        ultrasonicDistanceCm = (durationMicroseconds * 0.0343) / 2;
        snprintf(distancePrintout, 20, "%.2f cm     ", ultrasonicDistanceCm);
    } else {
        ultrasonicDistanceCm = -1; 
        snprintf(distancePrintout, 20, "??? cm     ");
    }

    if (ultrasonicSerialEnable) {
        elapsedTimeMicroseconds = micros() - startTimeMicroseconds;
        //Serial.println(elapsedTimeMicroseconds);
        Serial.println(distancePrintout);
    }
    ultrasonicDistanceNearestCm = (int)(ultrasonicDistanceCm + 0.5); 
    
    return elapsedTimeMicroseconds;
}

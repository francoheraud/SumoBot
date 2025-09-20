// Ultrasonic sensor code
// Allan Wu (23810308)
// 20 September 2025
#include <Arduino.h>

#define ULTRASONIC_TRIGGER 1
#define ULTRASONIC_ECHO 2

int ultrasonicDistanceNearestCm = -1;

// Set ultrasonicSerialEnable to 1 to enable printout to the serial monitor
int ultrasonicSerialEnable;

// Set the maximum time we will wait for the echo pulse: this determines what is "OUT OF RANGE" for the sensor!
// From my testing, 15000 us timeout limits the distane range to ~200 cm
const static int timeoutMicroseconds = 15000;

// Time to wait before running the loop again, note that maximum polling rate is 50 Hz => 20,000 us delay between reads
const static int pollDelayMicroseconds = 50000;

void setupUltrasonicSensor(bool outputEnable) {
    pinMode(ULTRASONIC_TRIGGER, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);
    if (outputEnable) {
		Serial.begin(115200);
		ultrasonicSerialEnable = 1;
	}
}

int pollUltrasonicSensor() {
    static unsigned long durationMicroseconds;
    static double distanceCm;
    static unsigned long startTimeMicroseconds, elapsedTimeMicroseconds = 0;
    static char distancePrintout[20];

    digitalWrite(ULTRASONIC_TRIGGER, LOW); // Write low to ensure a clean trigger pulse
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIGGER, HIGH); // Trigger pulse must be high for minimum 10 us
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIGGER, LOW);

    startTimeMicroseconds = micros();

    // Call pulseIn() to read the echo pulse duration, if timeout occurs duration is instead set to 0
    durationMicroseconds = pulseIn(ULTRASONIC_ECHO, HIGH, timeoutMicroseconds);

    if (durationMicroseconds > 0) {
        // Calculation: distance = v*t = (343 m/s) * (100 c/m) * (time in us) * (0.000001 s / us) / 2
        distanceCm = (durationMicroseconds * 0.0343) / 2;
        snprintf(distancePrintout, 20, "%.2f cm     ", distanceCm);
    } else {
        distanceCm = -1; // Note that non-positive values indicate no enemy found
        snprintf(distancePrintout, 20, "??? cm     ");
    }

    // Output to serial monitor
    if (ultrasonicSerialEnable) {
        elapsedTimeMicroseconds = micros() - startTimeMicroseconds;
        Serial.println(elapsedTimeMicroseconds);
        Serial.println(distancePrintout);
    }
    ultrasonicDistanceNearestCm = (int)(distanceCm + 0.5); // Distance rounded to nearest cm
    
	return elapsedTimeMicroseconds; // Return elapsed time

    // Wait before the next measurement
    // if (pollDelayMicroseconds > elapsedTimeMicroseconds)
        // delayMicroseconds(pollDelayMicroseconds - elapsedTimeMicroseconds);
}
// Combined line detection and ultrasonic sensor code
// Author: Allan Wu (23810308)

#include "Sensors.h"
#include <Arduino.h>

void initSensors() // Please note that the Line Detector pin must support ADC
{
    pinMode(LEFT_TRIGGER, OUTPUT);
    pinMode(LEFT_ECHO, INPUT);
    pinMode(RIGHT_TRIGGER, OUTPUT);
    pinMode(RIGHT_ECHO, INPUT);
    pinMode(LINEDETECTOR_DAC, INPUT);
}

void detectLine(Sensors_t *sensors)
{
    int binarycode[4]; // For debug purposes we can check the values stored in here
    int analogValue = analogRead(LINEDETECTOR_DAC);
	int *ptrs[4] = {&sensors->bottomRight, &sensors->bottomLeft, &sensors->topRight, &sensors->topLeft};
    int encoding = 16; // Defaults to '1111'
    for (int i = 0; i < 16; i++) {
        if (analogValue < ADCLookup[i]) {
            encoding = i;
            break;
        }
    }
    for (int j = 0; j < 4; j++) {
        int remainder = encoding % 2;
        encoding /= 2;
        binarycode[j] = remainder;
        *ptrs[j] = remainder;
    }
}

void pollDistance(int side, Sensors_t *sensors)
{
    double ultrasonicDistanceCm;
	unsigned long durationMicroseconds;
	int echoPin, triggerPin;
	int *sensorPtr;

	switch (side) {
	case 0:
		echoPin = LEFT_ECHO;
		triggerPin = LEFT_TRIGGER;
		sensorPtr = &sensors->leftCm;
		break;
	case 1:
		echoPin = RIGHT_ECHO;
		triggerPin = RIGHT_TRIGGER;
		sensorPtr = &sensors->rightCm;
		break;
	}

	// Send trigger pulse
    digitalWrite(triggerPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH); 
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    // Call pulseIn() to read the echo pulse duration, if timeout occurs duration is instead set to 0
    durationMicroseconds = pulseIn(echoPin, HIGH, ULTRASONIC_TIMEOUT_US);

    if (durationMicroseconds > 0) {
        // Calculation: distance = v*t = (343 m/s) * (100 c/m) * (time in us) * (0.000001 s / us) / 2
        ultrasonicDistanceCm = (durationMicroseconds * 0.0343) / 2;
		// Round to nearest centimetre
    	*sensorPtr = (int)(ultrasonicDistanceCm + 0.5); 
    } else {
        *sensorPtr = OUT_OF_RANGE;
    }
}
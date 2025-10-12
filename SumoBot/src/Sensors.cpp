// Combined line detection and ultrasonic sensor code
// Author: Allan Wu (23810308)

#include "Sensors.h"
#include <Arduino.h>

int lineBinaryCode[4] = {0};

// Convert between analog voltage reading and binary codes 0000 to 1111
// (topLeft, topRight, bottomLeft, bottomRight): 0 = WHITE, 1 = BLACK
int ADCLookup[16] = {
    1499, // 0000
    1599, // 0001
    1799, // 0010
    1899, // 0011

    2049, // 0100
    2199, // 0101
    2299, // 0110
    2499, // 0111

    2699, // 1000
    2839, // 1001
    2999, // 1010
    3149, // 1011

    3349, // 1100
    3499, // 1101
    3799, // 1110
    4096, // 1111
};

void initSensors() // Please note that the Line Detector pin must support ADC
{
    pinMode(LEFT_TRIGGER, OUTPUT);
    pinMode(LEFT_ECHO, INPUT);
    pinMode(RIGHT_TRIGGER, OUTPUT);
    pinMode(RIGHT_ECHO, INPUT);
    pinMode(LINEDETECTOR_DAC, INPUT);
}

// Recalibration requires passing in an array of 16 analog readings
// Input array should start at 0000 (all white) and end with 1111 (all black)
void recalibrateADC(int analogReadings[16])
{
    int curr, next;
    for (int i = 0; i < 16; i++) {
        curr = analogReadings[i];
        next = (i+1 < 16) ? analogReadings[i+1] : 4096;
        ADCLookup[i] = (int)(curr + next)/2;
    };
}

Sensors_t *Sensors()
{
	Sensors_t *ptr = (Sensors_t *)malloc(sizeof(Sensors_t));
	ptr->leftCm = OUT_OF_RANGE;
	ptr->rightCm = OUT_OF_RANGE;

	ptr->analogReading = 4095;
	ptr->topLeft = 1;
	ptr->topRight = 1;
	ptr->bottomLeft = 1;
	ptr->bottomRight = 1;
	return ptr;
}

void detectLine(Sensors_t *sensors)
{
    int analogValue = analogRead(LINEDETECTOR_DAC);
	sensors->analogReading = analogValue;
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
        lineBinaryCode[j] = remainder;
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
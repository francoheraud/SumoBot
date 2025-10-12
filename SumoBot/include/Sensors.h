#ifndef SENSORS_H
#define SENSORS_H
#include <Arduino.h>

// Pins may need to be changed
#define LEFT_TRIGGER			18
#define LEFT_ECHO				17
#define RIGHT_TRIGGER			43
#define RIGHT_ECHO				44
#define LINEDETECTOR_DAC		16

#define LEFT_ULTRASONIC 	0
#define RIGHT_ULTRASONIC 	1
#define OUT_OF_RANGE 		-1

// Set the maximum time we will wait for the echo pulse: this determines what is "OUT OF RANGE" for the sensor!
// From my testing, 15000 us timeout limits the distance range to ~200 cm
#define ULTRASONIC_TIMEOUT_US	15000

// Lookup table
extern int ADCLookup[16];

// Global variable for debugging purposes
extern int lineBinaryCode[4];

typedef struct {
	// Ultrasonic sensor distances stored to the nearest cm, -1 is OUT_OF_RANGE
	int leftCm;
	int rightCm;

	// Line detector booleans: 0 = WHITE, 1 = BLACK 
	// Should be interpreted as (Is this corner of the SumoBot within the arena?)
    int analogReading;
    int topLeft;
    int topRight;
    int bottomLeft;
    int bottomRight;
} Sensors_t;


// Pin setup
void initSensors();

// Calculates ADC lookup table using an array of analog readings
void recalibrateADC(int analogReadings[16]);

// Constructor returns a pointer to sensor struct
Sensors_t *Sensors();

void detectLine(Sensors_t *sensors);

// pollDistance(LEFT_ULTRASONIC, sensors) to get the left distance in cm
// pollDistance(RIGHT_ULTRASONIC, sensors) to get the right distance in cm
void pollDistance(int side, Sensors_t *sensors);

#endif
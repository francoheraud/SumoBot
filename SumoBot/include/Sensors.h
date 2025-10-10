#ifndef SENSORS_H
#define SENSORS_H
#include <Arduino.h>

// Pins may need to be changed
#define LEFT_TRIGGER			43
#define LEFT_ECHO				44
#define RIGHT_TRIGGER			18
#define RIGHT_ECHO				17
#define LINEDETECTOR_DAC		16

#define LEFT_ULTRASONIC 0
#define RIGHT_ULTRASONIC 1
#define OUT_OF_RANGE -1

// Set the maximum time we will wait for the echo pulse: this determines what is "OUT OF RANGE" for the sensor!
// From my testing, 15000 us timeout limits the distane range to ~200 cm
#define ULTRASONIC_TIMEOUT_US	15000

typedef struct {
	// Ultrasonic sensor distances stored to the nearest cm, -1 is OUT_OF_RANGE
	int leftCm;
	int rightCm;

	// Line detector booleans: 0 = WHITE, 1 = BLACK 
	// Should be interpreted as (Is this corner of the SumoBot within the arena?)
    int topLeft;
    int topRight;
    int bottomLeft;
    int bottomRight;
} Sensors_t;

// Convert between analog voltage reading and binary codes 0000 to 1111
// (topLeft, topRight, bottomLeft, bottomRight): 0 = WHITE, 1 = BLACK
int ADCLookup[16] = {
    300, // 0000
    550, // 0001
    800, // 0010
    950, // 0011

    1200, // 0100
    1400, // 0101
    1750, // 0110
    2049, // 0111

    2050, // 1000
    2250, // 1001
    2500, // 1010
    2700, // 1011

    3000, // 1100
    3200, // 1101
    3600, // 1110
    4096 // 1111
};

void initSensors();

void detectLine(Sensors_t *sensors);

// pollDistance(LEFT_ULTRASONIC, sensors) to get the left distance in cm
// pollDistance(RIGHT_ULTRASONIC, sensors) to get the right distance in cm
void pollDistance(int side, Sensors_t *sensors);

#endif
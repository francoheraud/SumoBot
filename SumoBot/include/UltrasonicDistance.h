// Ultrasonic header file
// Allan Wu (23810308)
// Updated 21 September 2025
#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>


#define ULTRASONIC1_TRIGGER         43
#define ULTRASONIC1_ECHO            44

#define ULTRASONIC2_TRIGGER         18
#define ULTRASONIC2_ECHO            17

// Set the maximum time we will wait for the echo pulse: this determines what is "OUT OF RANGE" for the sensor!
// From my testing, 15000 us timeout limits the distane range to ~200 cm
#define ULTRASONIC_TIMEOUT_US       15000 // us


// Global data variables that can be used in other files
// Note that sentinel value -1 indicates "NO ENEMY FOUND"
extern double ultrasonicDistanceCm;
extern int ultrasonicDistanceNearestCm;

// Set ultrasonicSerialEnable to 1 to enable printout to the serial monitor
extern bool ultrasonicSerialEnable;


// Time to wait before running the loop again, note that maximum polling rate is 50 Hz => 20,000 us delay between reads
// (currently not used)
extern int pollDelayMicroseconds;

// Must run this before polling the ultrasonic sensor, can use different pins and set a longer timeout if needed
void setupUltrasonicSensor(bool serialEnable = false);

// Currently this returns the elapsed time in microseconds and updates global variables (implementation can be changed)
int pollUltrasonicSensor(void);

#endif
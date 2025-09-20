// Ultrasonic header file
// Allan Wu (23810308)
// 21 September 2025
#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// Global variable that can be referenced elsewhere, note that a negative value indicates no enemy found
extern int ultrasonicDistanceNearestCm;

// Set ultrasonicSerialEnable to 1 using setupUltrasonicSensor(), to enable printout to the serial monitor
extern int ultrasonicSerialEnable;

void setupUltrasonicSensor(bool outputEnable = false);

unsigned long pollUltrasonicSensor();

#endif

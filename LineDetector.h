// Line detector header
// Original by Anuvrat, with modifications by Allan
// 24 September 2025
#ifndef LINEDETECTOR_H
#define LINEDETECTOR_H

// Set important constants using the following function, needed to use readAndDisplayIrSensor()
void setupLineDetector(int irDigitalPin, int irAnalogPin, int irLineThreshold);
// Read an IR sensor's analog pin and RETURN boolean (TRUE -> white edge detected, FALSE -> black)
bool irDetectWhiteLine(int irAnalogPin, int irLineThreshold);
// Same as above except we use the default IR_LINE_THRESHOLD value
bool irDetectWhiteLine(int irAnalogPin);
// Read the IR sensor's sensor (analog and digital pins) and print strings to TFT display and/or serial monitor
void readAndDisplayIrSensor();

// Define output boolean variable, set by readAndDisplayIrSensor()
extern bool irWhiteLineDetected;

// Define 'constant' pins and variables which should be set only once
extern int IR_DIGITAL_PIN;
extern int IR_ANALOG_PIN;
extern int IR_LINE_THRESHOLD; 

// The booleans enable outputs of readAndDisplayIrSensor and are usually true
extern bool IR_ENABLE_TFT;
extern bool IR_ENABLE_SERIAL;

#endif

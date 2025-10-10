#include "LineDetectors.h"
#include <Arduino.h>

void detectLine(int lineDetectorAnalogPin, Line_t *detectors) // Pin must support ADC
{
    int binarycode[4]; // For debug purposes
    int analogValue = analogRead(lineDetectorAnalogPin);
	int *ptrs[4] = {&detectors->bottomRight, &detectors->bottomLeft, &detectors->topRight, &detectors->topLeft};
    static int encoding;
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
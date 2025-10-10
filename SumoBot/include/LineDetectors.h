#ifndef LINEDETECTORS_H
#define LINEDETECTORS_H
#include <Arduino.h>

typedef struct {
    int topLeft;
    int topRight;
    int bottomLeft;
    int bottomRight;
} Line_t;

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

void detectLine(int lineDetectorAnalogPin, Line_t *detectors);

#endif
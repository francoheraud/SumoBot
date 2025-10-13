// Combined line detection and ultrasonic sensor code
// Author: Allan Wu (23810308)

#include "Sensors.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Preferences.h>

Preferences botSettings; 

// Convert between analog voltage reading and binary codes 0000 to 1111
// (frontLeft, frontRight, rearLeft, rearRight): 0 = WHITE, 1 = BLACK
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

const char *ADCStrings[16] = {
    "0000 NO_LINE ", // 0000
    "0001 R_RIGHT ", // 0001
    "0010 R_LEFT  ", // 0010
    "0011 _REAR_  ", // 0011

    "0100 F_RIGHT ", // 0100
    "0101 _RIGHT_ ", // 0101
    "0110 INVALID1", // 0110
    "0111 R_RIGHT2", // 0111

    "1000 F_LEFT  ", // 1000
    "1001 INVALID2", // 1001
    "1010 _LEFT_  ", // 1010
    "1011 R_LEFT_2", // 1011

    "1100 _FRONT_ ", // 1100
    "1101 F_RIGHT2", // 1101
    "1110 F_LEFT_2", // 1110
    "1111 OUTSIDE ", // 1111
};

void initSensors() // Please note that the Line Detector pin must support ADC
{
    pinMode(LEFT_TRIGGER, OUTPUT);
    pinMode(LEFT_ECHO, INPUT);
    pinMode(RIGHT_TRIGGER, OUTPUT);
    pinMode(RIGHT_ECHO, INPUT);
    pinMode(LINEDETECTOR_DAC, INPUT);

	botSettings.begin("botSettings", false);
    for (int i = 0; i < 16; i++) {
	    if (!botSettings.isKey(ADCStrings[i]))
		    botSettings.putInt(ADCStrings[i], ADCLookup[i]);
        else 
		    ADCLookup[i] = botSettings.getInt(ADCStrings[i]);
    }
}

Sensors_t *Sensors() // Java-style "constructor"
{
	Sensors_t *ptr = (Sensors_t *)malloc(sizeof(Sensors_t));
	ptr->leftCm = OUT_OF_RANGE;
	ptr->rightCm = OUT_OF_RANGE;

	ptr->analogReading = 0;
	ptr->frontLeft = 0;
	ptr->frontRight = 0;
	ptr->rearLeft = 0;
	ptr->rearRight = 0;
	return ptr;
}

// Display ADCLookup values
void printADCLookup(TFT_eSPI *tft, uint32_t colour)
{
    tft->setTextSize(1);
    tft->setTextColor(colour, TFT_BLACK);
    tft->fillScreen(TFT_BLACK);
    for (int i = 0; i < 16; i++) {
        tft->setCursor(0, i*10);
        tft->printf("%s: ADCLookup[%2d] = %d ", ADCStrings[i], i, ADCLookup[i]);
        delay(100);
    };
    delay(4000);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->fillScreen(TFT_BLACK);
}

// Resets ADC lookup table and flash memory to hardcoded values
void resetADCLookup(TFT_eSPI *tft)
{
    ADCLookup[0] = 1499; // 0000
    ADCLookup[1] = 1599; // 0001
    ADCLookup[2] = 1799; // 0010
    ADCLookup[3] = 1899; // 0011

    ADCLookup[4] = 2049; // 0100
    ADCLookup[5] = 2199; // 0101
    ADCLookup[6] = 2299; // 0110
    ADCLookup[7] = 2499; // 0111

    ADCLookup[8] = 2699; // 1000
    ADCLookup[9] = 2839; // 1001
    ADCLookup[10] = 2999; // 1010
    ADCLookup[11] = 3149; // 1011

    ADCLookup[12] = 3349; // 1100
    ADCLookup[13] = 3499; // 1101
    ADCLookup[14] = 3799; // 1110
    ADCLookup[15] = 4096; // 1111

    for (int i = 0; i < 16; i++) botSettings.putInt(ADCStrings[i], ADCLookup[i]);

    printADCLookup(tft, TFT_RED);
}

void recalibrateADC_GUI(TFT_eSPI *tft)
{
    int calibrationStage = 0;
    int currentReading = 0;
    int prevL = 0, currL = 0;
    int analogReadings[16] = {0};

    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->setTextSize(2);
    tft->setRotation(3);

    while (calibrationStage < 16) {
        currL = !digitalRead(14);
        currentReading = analogRead(LINEDETECTOR_DAC);
        tft->setCursor(5,10);
        tft->printf("Calibrating ADC (%d/15)", calibrationStage);

        tft->setCursor(5,40);
        tft->printf("State: %s      ", ADCStrings[calibrationStage]);

        if (calibrationStage == 6 || calibrationStage == 9) {
            tft->setCursor(5,85);
            tft->printf("Analog reading: N/A    ");
            tft->setCursor(5,145);
            tft->printf("<- Press R to continue     ");
        } else {
            tft->setCursor(5,85);
            tft->printf("Analog reading: %d    ", currentReading);
            tft->setCursor(5,145);
            tft->printf("<- Press R to record   ");
        }

        if (prevL && !currL) {
            analogReadings[calibrationStage] = currentReading;
            if (calibrationStage == 6 || calibrationStage == 9) {
                delay(100);
            } else {
                tft->setTextColor(TFT_GREEN, TFT_BLACK);
                tft->setCursor(5,85);
                tft->printf("Recorded value: %d      ", currentReading);
                delay(400);
                tft->setTextColor(TFT_WHITE, TFT_BLACK);
            }
            calibrationStage++;
        }

        prevL = currL;
        delay(100);
    }

    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    tft->setTextSize(1);
    int curr, next;
    for (int i = 0; i < 16; i++) {
        curr = analogReadings[i];
        next = (i+1 < 16) ? analogReadings[i+1] : 4096;
        if (i == 6 || i == 9) ADCLookup[i] = (analogReadings[i-1] + next)/2;
        else ADCLookup[i] = (curr + next)/2;
		botSettings.putInt(ADCStrings[i], ADCLookup[i]);
        tft->setCursor(0, i*10);
        tft->printf("%s: ADCLookup[%2d] = %d ", ADCStrings[i], i, ADCLookup[i]);
        delay(200);
    };
    tft->printf(" Done! ");
    delay(4000);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
	tft->fillScreen(TFT_BLACK);
}

void detectLine(Sensors_t *sensors)
{
    int analogValue = analogRead(LINEDETECTOR_DAC);
	sensors->analogReading = analogValue;
	int *ptrs[4] = {&sensors->rearRight, &sensors->rearLeft, &sensors->frontRight, &sensors->frontLeft};
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
        *ptrs[j] = remainder;
    }
}

void pollDistance(Sensors_t *sensors)
{
    static int currSensor = LEFT_ULTRASONIC; // LEFT = 0, RIGHT = 1
	double ultrasonicDistanceCm;
	unsigned long durationMicroseconds;
	int echoPin, triggerPin;
	int *sensorPtr;

	switch (currSensor) {
        case LEFT_ULTRASONIC:
            echoPin = LEFT_ECHO;
            triggerPin = LEFT_TRIGGER;
            sensorPtr = &sensors->leftCm;
            break;
        case RIGHT_ULTRASONIC:
            echoPin = RIGHT_ECHO;
            triggerPin = RIGHT_TRIGGER;
            sensorPtr = &sensors->rightCm;
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
    // Alternate between the left and right ultrasonic sensor
    currSensor = !currSensor;
}
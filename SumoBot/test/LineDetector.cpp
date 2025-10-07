// Line detector code
// Original by Anuvrat, with modifications by Allan
// 24 September 2025

#include <TFT_eSPI.h>

// Assume tft is initialised somewhere else
extern TFT_eSPI tft;
// If tft has NOT been initialised uncomment the following line
// TFT_eSPI tft = TFT_eSPI();

// These are default values, they may be overriden using the setup function
int IR_DIGITAL_PIN = 21;
int IR_ANALOG_PIN = 2;
int IR_LINE_THRESHOLD = 500;
// Anuvrat's tests: white line value was around 200 and the black one was around 2000

const int IR_DELAY_MILLIS = 200;

// TFT and/or serial monitor outputs can be turned off
bool IR_ENABLE_TFT = true;
bool IR_ENABLE_SERIAL = true;

bool irWhiteLineDetected = false;

void setupLineDetector(int irDigitalPin, int irAnalogPin, int irLineThreshold) {
    IR_DIGITAL_PIN = irDigitalPin;
    IR_ANALOG_PIN = irAnalogPin;
    IR_LINE_THRESHOLD = irLineThreshold;

    // Setup pins
    pinMode(IR_DIGITAL_PIN, INPUT);
    pinMode(IR_ANALOG_PIN, INPUT);
    
    char buffer[100];
    sprintf(buffer, "THRESH=%d D0:%d A0:%d", IR_LINE_THRESHOLD, IR_DIGITAL_PIN, IR_ANALOG_PIN);
    if (IR_ENABLE_SERIAL) {
        Serial.begin(115200);
        delay(1000);
        Serial.println("IR Sensor Ready");
        Serial.println(buffer);
    }
    
    // Initialize TFT
    if (IR_ENABLE_TFT) {
        tft.setRotation(3); // Can be 1 or 3
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(TFT_SILVER, TFT_BLACK);
        tft.drawString(buffer, 10, 15);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
}

// Barebones implementation with no printing
bool irDetectWhiteLine(int irAnalogPin, int irLineThreshold) {
    int analogValue = analogRead(irAnalogPin);
    return (analogValue < irLineThreshold);
}

bool irDetectWhiteLine(int irAnalogPin) {
    int analogValue = analogRead(irAnalogPin);
    return (analogValue < IR_LINE_THRESHOLD);
}

void readAndDisplayIrSensor() {
    // Read IR sensor
    int digitalValue = digitalRead(IR_DIGITAL_PIN);
    int analogValue = analogRead(IR_ANALOG_PIN);
    static char buffer[50];
    
    if (IR_ENABLE_TFT) {
        sprintf(buffer, "Digital: %d  ", digitalValue);
        tft.drawString(buffer, 10, 50);
        sprintf(buffer, "Analog: %d  ", analogValue);
        tft.drawString(buffer, 10, 80);
    }
    
    irWhiteLineDetected = (analogValue < IR_LINE_THRESHOLD);

    String surface = (irWhiteLineDetected) ? "WHITE" : "BLACK";
    
    if (IR_ENABLE_SERIAL) {
        Serial.printf("Digital: %d | Analog: %d | Surface: %s\n",
                      digitalValue, analogValue, surface.c_str());
        if (irWhiteLineDetected) Serial.println("WHITE EDGE DETECTED - REVERSE!");
    }
    
    if (IR_ENABLE_TFT) {
        if (analogValue < IR_LINE_THRESHOLD) { // "WHITE"
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        } else { // "BLACK"
            tft.setTextColor(TFT_RED, TFT_BLACK);
        }
        sprintf(buffer, "Surface: %s  ", surface);
        tft.drawString(buffer, 10, 110);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        
        // Visual bar
        int barWidth = map(analogValue, 0, 4095, 0, 320);
        tft.fillRect(0, 165, 320, 170, TFT_WHITE);
        if (barWidth > 0) {
            tft.fillRect(0, 165, barWidth, 170, TFT_CYAN);
        }
    }   
    
    if (IR_ENABLE_TFT || IR_ENABLE_SERIAL) 
        delay(IR_DELAY_MILLIS);
}





// BASIC DRIVER CODE (copy and paste this into main.cpp)
/*
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "LineDetector.h"

TFT_eSPI tft = TFT_eSPI();

void setup() {
    tft.init();
    setupLineDetector(21, 2, 500);
};

void loop() {
    readAndDisplayIrSensor();
};*/

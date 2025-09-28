#include <Arduino.h>
#include "motor.h"

// Motor driver pins (adjust if needed)
#define IN1A 2
#define IN2A 3
#define PWMA 5
#define ENCA 18

#define IN1B 4
#define IN2B 7
#define PWMB 6
#define ENCB 19

void runMotion(Direction dir, SpeedLevel speed, unsigned long durationMs) {
    move(dir, speed);
    unsigned long startTime = millis();
    while (millis() - startTime < durationMs) {
        updateMotors();  // continuously run PID
    }
    stopMotors();
    delay(500); // short pause between motions
}

void setup() {
    Serial.begin(115200);

    // Initialize motors and encoders
    initMotors(IN1A, IN2A, PWMA, ENCA, IN1B, IN2B, PWMB, ENCB);
}

void loop() {
    // Forward at LOW, MEDIUM, HIGH
    runMotion(FORWARD, MIN, 2000);
    runMotion(FORWARD, MEDIUM, 2000);
    runMotion(FORWARD, MAX, 2000);

    // Reverse at MEDIUM
    runMotion(REVERSE, MEDIUM, 2000);

    // Turn Right and Left at LOW
    runMotion(RIGHT, MIN, 2000);
    runMotion(LEFT, MAX, 2000);

    // Rotate 360 CW and CCW at MEDIUM
    runMotion(ROTATE_CW, MEDIUM, 2000);
    runMotion(ROTATE_CCW, MEDIUM, 2000);

    // Stop after testing
    stopMotors();
    while (1) {
        updateMotors();  // keep PID running at zero speed
    }
}

#include "Motor.h"
#include "Encoder.h"

// Motor pins
static int IN1A, IN2A, PWMA;
static int IN1B, IN2B, PWMB;

// PID parameters
static float Kp = 0.8, Ki = 0.1, Kd = 0.05;

// PID control variables
static long lastCountA = 0, lastCountB = 0;
static float integralA = 0, integralB = 0;
static float lastErrorA = 0, lastErrorB = 0;
static int targetSpeed = 0; // ticks/sec
static Direction currentDir = FORWARD;

// Convert speed level to target ticks/sec
static int speedToTargetTicks(SpeedLevel s) {
    switch(s) {
        case MIN: return 50;
        case MEDIUM: return 120;
        case MAX: return 255;
    }
    return 0;
}

void initMotors(int in1A, int in2A, int pwmA, int encA,
                int in1B, int in2B, int pwmB, int encB) {
    IN1A = in1A; IN2A = in2A; PWMA = pwmA;
    IN1B = in1B; IN2B = in2B; PWMB = pwmB;

    pinMode(IN1A, OUTPUT); pinMode(IN2A, OUTPUT); pinMode(PWMA, OUTPUT);
    pinMode(IN1B, OUTPUT); pinMode(IN2B, OUTPUT); pinMode(PWMB, OUTPUT);

    initEncoderA(encA);
    initEncoderB(encB);
}

void move(Direction dir, SpeedLevel speed) {
    currentDir = dir;
    targetSpeed = speedToTargetTicks(speed);

    switch(dir) {
        case FORWARD:
            digitalWrite(IN1A,HIGH); digitalWrite(IN2A,LOW);
            digitalWrite(IN1B,HIGH); digitalWrite(IN2B,LOW);
            break;
        case REVERSE:
            digitalWrite(IN1A,LOW); digitalWrite(IN2A,HIGH);
            digitalWrite(IN1B,LOW); digitalWrite(IN2B,HIGH);
            break;
        case RIGHT: // pivot right
            digitalWrite(IN1A,HIGH); digitalWrite(IN2A,LOW);
            digitalWrite(IN1B,LOW); digitalWrite(IN2B,LOW);
            break;
        case LEFT:  // pivot left
            digitalWrite(IN1A,LOW); digitalWrite(IN2A,LOW);
            digitalWrite(IN1B,HIGH); digitalWrite(IN2B,LOW);
            break;
        case ROTATE_CW:
            digitalWrite(IN1A,HIGH); digitalWrite(IN2A,LOW);
            digitalWrite(IN1B,LOW); digitalWrite(IN2B,HIGH);
            break;
        case ROTATE_CCW:
            digitalWrite(IN1A,LOW); digitalWrite(IN2A,HIGH);
            digitalWrite(IN1B,HIGH); digitalWrite(IN2B,LOW);
            break;
    }
}

void stopMotors() {
    analogWrite(PWMA,0);
    analogWrite(PWMB,0);
}

void updateMotors() {
    static unsigned long lastTime = 0;
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0; // seconds
    if (dt < 0.05) return; // update every 50ms
    lastTime = now;

    long countA = getEncoderCountA();
    long countB = getEncoderCountB();

    long deltaA = countA - lastCountA;
    long deltaB = countB - lastCountB;

    lastCountA = countA;
    lastCountB = countB;

    float actualSpeedA = deltaA / dt;
    float actualSpeedB = deltaB / dt;

    // PID for A
    float errorA = targetSpeed - actualSpeedA;
    integralA += errorA * dt;
    float derivativeA = (errorA - lastErrorA) / dt;
   
    float outputA = Kp*errorA + Ki*integralA + Kd*derivativeA;
    lastErrorA = errorA;

    // PID for B
    float errorB = targetSpeed - actualSpeedB;
    integralB += errorB * dt;
    float derivativeB = (errorB - lastErrorB) / dt;
   
    float outputB = Kp*errorB + Ki*integralB + Kd*derivativeB;
    lastErrorB = errorB;

    // Update PWM Values
    int pwmA = constrain((int)outputA, 0, 255);
    int pwmB = constrain((int)outputB, 0, 255);

    analogWrite(PWMA, pwmA);
    analogWrite(PWMB, pwmB);
}

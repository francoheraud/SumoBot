// Pratyush + Allan + Franco

#include "Motor.h"
#include "Encoder.h"

static long lastCountA = 0, lastCountB = 0;
static uint8_t targetSpeed = 0;
static Direction currentDir = FORWARD;

static int speedToTargetTicks(Motor_t *motor) {
    switch(motor->desiredSpeed) {
        case MIN: return 50;
        case MEDIUM: return 120;
        case MAX: return 255;
    }
    return 0;
}

void initMotors(void) {
    pinMode(IN1A, OUTPUT); pinMode(IN2A, OUTPUT); pinMode(PWMA, OUTPUT);
    pinMode(IN1B, OUTPUT); pinMode(IN2B, OUTPUT); pinMode(PWMB, OUTPUT);
    initEncoderA(PWMA);
    initEncoderB(PWMB);
}

void move(Motor_t *motor) {
    switch(motor->dir) {
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

void stopMotors(void) {
    analogWrite(PWMA,0);
    analogWrite(PWMB,0);
}

void updateMotorSpeed(Motor_t *motor) {

    static unsigned long lastTime = millis();
    unsigned long dt = (millis() - lastTime); 
    if (dt < SPEED_POLLING_PERIOD) return; 

    long countA = getEncoderCountA();
    long countB = getEncoderCountB();

    long deltaA = countA - lastCountA;
    long deltaB = countB - lastCountB;

    lastCountA = countA;
    lastCountB = countB;

    motor->speedA = deltaA / dt;
    motor->speedB = deltaB / dt;
    lastTime = millis();
}

// WIP
void updateMotors(Motor_t *motor) {
    updateMotorSpeed(motor);

    // 1. On-off control for adhering to a target speed
    targetSpeed = speedToTargetTicks(motor);
    motor->pwmA = (motor->speedA < targetSpeed) ? 255 : 0; 
    motor->pwmB = (motor->speedB < targetSpeed) ? 255 : 0;

    // 2. Proportional control for synchronising motor speeds (plz tune kp!)
    const float tolerance = 12.0f, kpSync = 0.1f;
    float error = motor->speedA - motor->speedB;

    if (motor->dir == FORWARD || motor->dir == REVERSE) {
        motor->pwmA -= (uint8_t)(kpSync * error);
        motor->pwmB += (uint8_t)(kpSync * error);
    }

    motor->pwmA = constrain(motor->pwmA, 0, 255);
    motor->pwmB = constrain(motor->pwmB, 0, 255);

    analogWrite(PWMA, motor->pwmA);
    analogWrite(PWMB, motor->pwmB);

}
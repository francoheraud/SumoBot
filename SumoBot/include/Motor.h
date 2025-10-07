#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>

#define SPEED_POLLING_PERIOD        50 // ms

#define IN1A    1
#define IN1B    2
#define IN2A    3
#define IN2B    10

#define PWMA    11
#define PWMB    12

// max recorded speed in ticks/s experimentally?
const float maxSpeed = 1.0f;


enum SpeedLevel { 
    MIN,
    MEDIUM, 
    MAX 
};

enum Direction { 
    FORWARD, 
    REVERSE, 
    LEFT, 
    RIGHT, 
    ROTATE_CW, 
    ROTATE_CCW 
};


typedef struct {
    float speedA;
    float speedB;
    uint8_t pwmA;
    uint8_t pwmB;

    SpeedLevel desiredSpeed;
    Direction dir;
} Motor_t;

void initMotors(void);

void move(Direction dir);
void stopMotors(void);

void updateMotorSpeed(Motor_t *motor);
void updateMotors(Motor_t *motor);

#endif

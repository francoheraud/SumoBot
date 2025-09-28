#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
#include <Arduino.h>

enum SpeedLevel { MIN, MEDIUM, MAX };
enum Direction { FORWARD, REVERSE, LEFT, RIGHT, ROTATE_CW, ROTATE_CCW };

void initMotors(int in1A, int in2A, int pwmA, int encA,
                int in1B, int in2B, int pwmB, int encB);

void move(Direction dir, SpeedLevel speed);
void stopMotors();
void updateMotors(); // PID loop, called continuously

#endif

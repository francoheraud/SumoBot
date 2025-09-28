#ifndef ENCODER_H
#define ENCODER_H
#include <Arduino.h>

void initEncoderA(int pin);
void initEncoderB(int pin);
long getEncoderCountA();
long getEncoderCountB();
void resetEncoders();

// ISR handlers
void IRAM_ATTR handleEncoderA();
void IRAM_ATTR handleEncoderB();
#endif

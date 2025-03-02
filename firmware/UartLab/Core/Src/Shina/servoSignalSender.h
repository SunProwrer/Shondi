#pragma once
#include "motorSignalSender.h"
#include <Servo.h>
#include <Arduino.h>

class ServoSignalSender : public MotorSignalSender {
public:
    ServoSignalSender(Servo& servo) {
        this->servo = &servo;

        this->minMicroseconds = 800;
        this->maxMicroseconds = 2300;
        this->minControlVal = 0;
        this->maxControlVal = 1024;
    }

    ~ServoSignalSender() { }

    void setControlVal(int16_t controlVal) {
        int16_t signal = convertToMicroseconds(controlVal);
        servo->writeMicroseconds(signal);
    }

    void setMinMicroseconds(int16_t minMicroseconds) {
        this->minMicroseconds = minMicroseconds;
    }

    void setMaxMicroseconds(int16_t maxMicroseconds) {
        this->maxMicroseconds = maxMicroseconds;
    }

protected:
    int16_t convertToMicroseconds(int16_t controlVal) {
        return map(controlVal, minControlVal, maxControlVal, minMicroseconds, maxMicroseconds);
    }

    Servo* servo;
    int16_t minMicroseconds;
    int16_t maxMicroseconds;
};
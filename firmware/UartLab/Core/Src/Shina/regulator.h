#pragma once
#include <stdint.h>
#include "minmaxable.h"

class Regulator : public MinMaxable {
public:
    virtual int16_t getControlVal() = 0;
    
    void setFactRPM(int16_t rpm) {
        this->factRPM = rpm;
    }

    void setAimRPM(int16_t rpm) {
        this->aimRPM = rpm;
    }
protected:
    int16_t factRPM;
    int16_t aimRPM;
    int16_t controlSignal;
};

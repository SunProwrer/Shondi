#pragma once
#include <stdint.h>
#include "minmaxable.h"

class RPMSetter : public MinMaxable {
public:
    virtual int16_t getAimRPM() = 0;
private:
    volatile int16_t aimRPM;
};

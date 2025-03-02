#pragma once
#include <stdint.h>
#include "minmaxable.h"

class RPMCounter : public MinMaxable {
public:
        virtual int16_t getFactRPM() = 0;
protected:
        volatile int16_t factRPM;
};

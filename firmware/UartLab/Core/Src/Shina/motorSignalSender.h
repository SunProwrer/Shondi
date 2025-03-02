#pragma once
#include <stdint.h>
#include "minmaxable.h"

class MotorSignalSender : public MinMaxable {
    public:
        virtual void setControlVal(int16_t controlVal) = 0;
    private:
        int16_t controlVal;
};

#pragma once
#include <stdint.h>

class MinMaxable {
public:
    void setMinControlVal(int16_t minControlVal) {
        this->minControlVal = minControlVal;
    }

    void setMaxControlVal(int16_t maxControlVal) {
        this->maxControlVal = maxControlVal;
    }
protected:
        uint16_t minControlVal;
        uint16_t maxControlVal;
};

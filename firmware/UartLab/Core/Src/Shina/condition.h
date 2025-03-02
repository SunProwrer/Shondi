#pragma once
#include <stdint.h>

struct Condition {
public:
    uint16_t factRPM;
    uint16_t aimRPM;
    uint16_t controlVal;
    uint8_t IOSettings;
    uint8_t libSettings;
};

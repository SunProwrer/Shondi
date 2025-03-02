#pragma once
#include <stdint.h>
#include <Arduino.h>

struct countData {
    volatile int16_t factRPM;
    uint8_t kolMagnetsPerRoute;
};

countData* data;

void setDataPtr(countData& dataPrt) {
    data = &dataPrt;
}

void countRPM() {
    static uint32_t lastTime = 0;
    uint32_t currentTime = micros();
    data->factRPM = (int16_t)(60 / ((float)(currentTime - lastTime) * data->kolMagnetsPerRoute) * 10000000);
    lastTime = currentTime;
}

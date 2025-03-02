#pragma once
#include "RPMCounter.h"
#include <AbstractCommands.h>
#include "count.h"

extern void countRPM();
extern void setDataPtr(countData& dataPrt);

class HallRPMCouner : public RPMCounter{
public:
    HallRPMCouner(uint8_t inputPort) {
        this->inputPort = inputPort;
        this->minControlVal = 0;
        this->maxControlVal = 1024;

        dataMas.kolMagnetsPerRoute = 6;

        setDataPtr(dataMas);
        attachInterrupt(digitalPinToInterrupt(inputPort), countRPM, RISING);
    }

    ~HallRPMCouner() { }

    int16_t getFactRPM() {
        return map(constrain(dataMas.factRPM, 0, 10000), 0, 10000, minControlVal, maxControlVal);
    }

    void setNumIntPerRoute(uint8_t ints) {
        dataMas.kolMagnetsPerRoute = ints;
    }
protected:
    countData dataMas;
    uint8_t inputPort;
};

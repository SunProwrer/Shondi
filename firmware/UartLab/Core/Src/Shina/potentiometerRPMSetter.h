#pragma once
#include "RPMSetter.h"
#include <AbstractCommands.h>

class PotentiometerRPMSetter : public RPMSetter {
public:
    PotentiometerRPMSetter(uint8_t inputPort) {
        this->inputPort = inputPort;
        this->minControlVal = 0;
        this->maxControlVal = 1024;
    }

    ~PotentiometerRPMSetter() { }

    void setAbstractCommands(AbstractCommands& hardware) {
        this->hardware = &hardware;
    }

    int16_t getAimRPM() {
        return map(hardware->readAnalogSignal(inputPort), 0, 1024, minControlVal, maxControlVal);
    }
protected:
    AbstractCommands* hardware;
    uint8_t inputPort;
};

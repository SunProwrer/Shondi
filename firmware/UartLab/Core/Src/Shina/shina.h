#pragma once
#include "condition.h"
#include "regulator.h"
#include "RPMCounter.h"
#include "RPMSetter.h"
#include "motorSignalSender.h"
//#include "IOModule.h"

class Shina {
public:
    Shina() {
        condition.aimRPM = 0;
        condition.controlVal = 0;
        condition.factRPM = 0;
        condition.IOSettings = 0b00000000;
        condition.libSettings = 0b00000000;
    }

    ~Shina() {
        if (condition.libSettings & 1) {
            delete regulator;
            delete counter;
            delete setter;
            delete signalSender;
//            delete ioModule;
        }
    }

    void tick() {
        condition.factRPM = counter->getFactRPM();
        condition.aimRPM = setter->getAimRPM();

        regulator->setAimRPM(condition.aimRPM);
        regulator->setFactRPM(condition.factRPM);
        condition.controlVal = regulator->getControlVal();

        signalSender->setControlVal(condition.controlVal);

//        ioModule->tick();
    }

    void setRegulator(Regulator& regulator) {
        this->regulator = &regulator;
    }

    void setRPMCounter(RPMCounter& counter) {
        this->counter = &counter;
    }

    void setRPMSetter(RPMSetter& setter) {
        this->setter = &setter;
    }

    void setMotorSignalSender(MotorSignalSender& signalSender) {
        this->signalSender = &signalSender;
    }

//    void setIOModule(IOModule& ioModule) {
//        this->ioModule = &ioModule;
//        this->ioModule->attachCondition(condition);
//    }
private:
    Condition condition;
    Regulator* regulator;
    RPMCounter* counter;
    RPMSetter* setter;
    MotorSignalSender* signalSender;
//    IOModule* ioModule;
};

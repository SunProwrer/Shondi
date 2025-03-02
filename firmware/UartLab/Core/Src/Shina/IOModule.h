#pragma once
#include <stdint.h>
#include "condition.h"
#include <AbstractConsole.h>

class IOModule {
public:
    virtual void setAbstractConsole(AbstractConsole& console) = 0;
    virtual void attachCondition(Condition& condition) = 0;
    virtual void tick() = 0;
};

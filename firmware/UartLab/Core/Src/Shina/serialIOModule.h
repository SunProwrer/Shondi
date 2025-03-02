#pragma once
#include <stdlib.h>
#include "IOModule.h"
#include "condition.h"
//#include <cstring.h>
#include <AbstractConsole.h>
#include <Arduino.h>

#define ONOFF 1
#define AIMRPM 2
#define FACTRPM 3
#define CONTROLSIGNAL 4
#define MODE 5

class SerialIOModule : public IOModule{
public:
    SerialIOModule() {
        this->period = 1000;
    }

    SerialIOModule(AbstractConsole& console) {
        this->console = &console;
    }

    ~SerialIOModule() { }

    void setAbstractConsole(AbstractConsole& console) {
        this->console = &console;
    }

    void attachCondition(Condition& condition) {
        this->condition = &condition;
        this->condition->IOSettings = this->condition->IOSettings | (0b0011111);
    }

    void setPeriod(uint16_t period) {
        this->period = period;
    }

    void tick() {
        if (isTurnOn()) {
            if (isConsoleMode()) {
                sendConsoleData();
            } else {
                sendPlotterData();
            }
        }
        readCondition();
    }

private:
    void readCondition() {
        if (Serial.available()) {
            char buf[24];
            uint8_t kol = Serial.readBytesUntil(';', buf, 23);
            buf[kol] = '\0';

            for (uint8_t i = 0; i < kol; i++) {
                console->println(i);
                char item = buf[i];
                char value[8];
                uint8_t j;
                for (j = 0; buf[++i] != ',' && buf[i] != '\0' ; j++) {
                    value[j] = buf[i];
                }
                if (j == 0) continue;
                
                value[i] = 0;
                int16_t intVal = atoi(value);
                switch (item) {
                case 'r':
                    condition->aimRPM = intVal;
                    break;
                case 'c':
                    setTurnOnOffSetting(intVal);
                    break;
                case 'a':
                    setPrintAimRPMSetting(intVal);
                    break;
                case 'f':
                    setPrintFactRPMSetting(intVal);
                    break;
                case 's':
                    setPrintControlValSetting(intVal);
                    break;
                case 'm':
                    setModeSetting(intVal);
                    break;
                
                default:
                    sendErrorMessage(item, value);
                    break;
                }
            }
        }
    }

    void sendConsoleData() {
        if (hasPassedPeriod()) {
            if (isPrintFactRPM()) sendFactRPM();
            if (isPrintAimRPM()) sendAimRPM();
            if (isPrintControlVal()) sendControlVal();
        }
    }

    void sendPlotterData() {
        if (isPrintFactRPM()) sendFactRPMPlotter();
        if (isPrintAimRPM()) sendAimRPMPlotter();
        if (isPrintControlVal()) sendControlValPlotter();
        console->println(" ");
    }

    void sendFactRPM() {
        sendDataWithMessage("Fact RPM: ", condition->factRPM);
    }

    void sendAimRPM() {
        sendDataWithMessage("Aim RPM: ", condition->aimRPM);
    }

    void sendControlVal() {
        sendDataWithMessage("Control value: ", condition->controlVal);
    }

    void sendFactRPMPlotter() {
        console->print(condition->factRPM);
        console->print(",");
    }

    void sendAimRPMPlotter() {
        console->print(condition->aimRPM);
        console->print(",");
    }

    void sendControlValPlotter() {
        console->print(condition->controlVal);
        console->print(",");
    }

    bool hasPassedPeriod() {
        static uint32_t lastTime = millis();
        if (millis() - lastTime <= period) {
            return false;
        }

        console->println("YES");
        lastTime = millis();
        return true;
    }

    bool isTurnOn() {
        return bitRead(condition->IOSettings, ONOFF);
    }

    bool isPrintAimRPM() {
        return bitRead(condition->IOSettings, AIMRPM);
    }

    bool isPrintFactRPM() {
        return bitRead(condition->IOSettings, FACTRPM);
    }

    bool isPrintControlVal() {
        return bitRead(condition->IOSettings, CONTROLSIGNAL);
    }

    bool isConsoleMode() {
        return bitRead(condition->IOSettings, MODE);
    }

    void setTurnOnOffSetting(bool val) {
        bitWrite(condition->IOSettings, ONOFF, val);
    }

    void setPrintAimRPMSetting(bool val) {
        bitWrite(condition->IOSettings, AIMRPM, val);
    }

    void setPrintFactRPMSetting(bool val) {
        bitWrite(condition->IOSettings, FACTRPM, val);
    }

    void setPrintControlValSetting(bool val) {
        bitWrite(condition->IOSettings, CONTROLSIGNAL, val);
    }

    void setModeSetting(bool val) {
        bitWrite(condition->IOSettings, MODE, val);
    }

    void sendDataWithMessage(const char* msg, int16_t val) {
        console->print(msg);
        console->println(val);
    }

    void sendErrorMessage(char item, char* value) {
        console->print("Invalid param: \"");
        console->print(item);
        console->print(value);
        console->println("\"");
    }
protected:
    Condition* condition;
private:
    AbstractConsole* console;
    uint16_t period = 1000;
};


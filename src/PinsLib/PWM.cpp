#include "PinsLib/PWM.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define PWM_PATH            "/sys/class/pwm/"
#define PWM_EXPORT_PATH     PWM_PATH"pwmchip2/"

#define PERIOD_FILENAME     "period"
#define DUTYCYCLE_FILENAME  "duty_cycle"
#define ENABLE_FILENAME     "enable"

namespace PinsLib {

    /**
     *
     * @param number The PWM number for the BBB
     */
    PWM::PWM(int number) : Pins(number, PWM_EXPORT_PATH) {
        ostringstream s;
        s << "pwm-2:" << number;
        this->name = string(s.str());
        this->path = PWM_PATH + this->name + "/";
    }

    int PWM::setPeriod(int period){
        return this->write(PERIOD_FILENAME, period);
    }

    int PWM::setDutyCycle(int dutyCycle){
        return this->write(DUTYCYCLE_FILENAME, dutyCycle);
    }


    PWM::~PWM() {
        this->unexportPin();
    }

    int PWM::getPeriod() {
        return stoi(this->read(PERIOD_FILENAME));
    }

    int PWM::getDutyCycle() {
        return stoi(this->read(DUTYCYCLE_FILENAME));
    }

    int PWM::setEnable(int enable) {
        return this->write(ENABLE_FILENAME, enable);
    }
}
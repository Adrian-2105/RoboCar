#ifndef PWM_H
#define PWM_H

#include <string>
#include <fstream>
#include "Pins.h"

namespace PinsLib {

    class PWM : virtual public Pins {
    private:

    public:
        // Constructor general que también exportará el pin indicado
        PWM(int number);

        // Destructor. También eliminará la exportación del pin
        virtual ~PWM();

        virtual int getNumber() { return number; }

        // Getters / Setters
        virtual int setPeriod(int period);
        virtual int getPeriod();

        virtual int setDutyCycle(int dutyCycle);
        virtual int getDutyCycle();

        virtual int setEnable(int enable);

    };

} /* namespace PinsLib */

#endif //PWM_H

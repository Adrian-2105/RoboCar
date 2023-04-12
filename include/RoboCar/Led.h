#ifndef ROBOCAR_LED_H
#define ROBOCAR_LED_H

#include "PinsLib/GPIO.h"

namespace RoboCar {

    class Led {
    private:
        // Pin asociado al LED
        PinsLib::GPIO *ledPin;
        bool on;

    public:
        Led(int number);

        ~Led();

        // Getters
        bool isOn() const;

        // Encendido y apagado de los pines
        void turnOn();
        void turnOff();
        void toggle();
    };

}

#endif //ROBOCAR_LED_H

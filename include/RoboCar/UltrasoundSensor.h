#ifndef ROBOCAR_ULTRASOUNDSENSOR_H
#define ROBOCAR_ULTRASOUNDSENSOR_H

#include "PinsLib/GPIO.h"

namespace RoboCar {

    class UltrasoundSensor {
    private:
        // Pines utilizados para realizar las mediciones
        PinsLib::GPIO *triggerPin;
        PinsLib::GPIO *echoPin;

    public:
        // Constructor. Inicializa los pines y configura el sensor
        // Nota: solo puede existir una instancia de sensor simultáneamente
        UltrasoundSensor();

        ~UltrasoundSensor();

        // Obtiene la distancia, en centímetros, que mide el sensor
        float getDistance();
    };

} /* namespace RoboCar */


#endif //ROBOCAR_ULTRASOUNDSENSOR_H

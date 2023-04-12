#ifndef ROBOCAR_WHEELMOTOR_H
#define ROBOCAR_WHEELMOTOR_H

#include "PinsLib/GPIO.h"
#include "PinsLib/PWM.h"
#include <vector>

// Macros auxiliares para el acceso al pair<int,int> con el mínimo y máximo valor de velocidad
#define minimum first
#define maximum second

using std::string;
using std::pair;

namespace RoboCar {

    // Enumerado con las ruedas que son posibles crear
    enum Wheel { LEFT = 1, RIGHT = 0};

    class WheelMotor {
    private:
        // Pines utilizados para la dirección de movimiento de la rueda
        PinsLib::GPIO *forwardPin;
        PinsLib::GPIO *backwardPin;

        // Pin PWM para especificar la velocidad de movimiento de la rueda
        PinsLib::PWM *speedPin;

        // Encoder (tacómetro) utilizado para medir la velocidad
        PinsLib::GPIO *encoderPin;

        // Parámetros respectivos a la velocidad y sus configuraciones
        std::vector<std::pair<int, int>> speeds;
        int minSpeed;
        int maxSpeed;

        // Variables generales para el funcionamiento de la rueda
        bool moving;
        bool calibrated;
        int dutyCycle;

    public:
        // Inicializa la rueda indicada (LEFT, RIGHT)
        // Nota: solo se puede crear una instancia de cada tipo
        WheelMotor(Wheel wheel);

        // Desvincula los pines de la rueda
        ~WheelMotor();

        // Funciones relativas al movimiento de la rueda
        void goForward();
        void goBackward();
        void stop();

        // Funciones para la regulación de las velocidades
        bool setSpeed(int speed);
        int getCurrentSpeed();
        void updateSpeed(int referenceSpeed);

        // Funciones de calibración
        pair<int, int> calibrate();
        bool saveCalibration(string filename);
        pair<int, int> loadCalibration(string filename);

    private:
        // Función utilizada para la regulación de la velocidad
        void setDutyCycle(int dutyCycle);
    };

} /* namespace RoboCar */

#endif //ROBOCAR_WHEELMOTOR_H

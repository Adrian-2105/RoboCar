#ifndef ROBOCAR_ROBOCAR_H
#define ROBOCAR_ROBOCAR_H

#include "Led.h"
#include "WheelMotor.h"
#include "UltrasoundSensor.h"

using namespace std;

namespace RoboCar {

    enum LEDS_COLOR {GREEN, RED};

    class RoboCar {
    private:
        // Sensores y actuadores utilizados
        WheelMotor* leftWheel;
        WheelMotor* rightWheel;
        UltrasoundSensor* ultrasoundSensor;
        Led* greenLed;
        Led* redLed;

        // Parámetros para el control de la velocidad
        int speed;
        int maxSpeed;
        int minSpeed;

    public:
        // Constructor. Inicializa sensores y pines necesarios para la configuración que hemos establecido
        // Nota: solo puede existir una misma instancia simultáneamente
        RoboCar();

        // Destructor. Libera todos los recursos utilizados por el coche
        ~RoboCar();

        // Funcionalidad respectiva al movimiento del vehículo
        void goForward();
        void goBackward();
        void goRight();
        void goRight(int angle);
        void goLeft();
        void goLeft(int angle);
        void rotateRight();
        void rotateRight(int angle);
        void rotateLeft();
        void rotateLeft(int angle);
        void stop();

        // Funciones para el control y gestión de la velocidad de movimiento
        void setSpeed(int speed);
        void setMaxSpeed();
        void setMinSpeed();
        void setMeanSpeed();
        int getSpeed() const;
        int getMaxSpeed() const;
        int getMinSpeed() const;
        void updateSpeed();

        // Funciones para la medida de distancias desde el vehículo al siguiente obstáculo
        float getDistance();

        // Funciones para el control de los leds
        void turnOnLed(LEDS_COLOR color);
        void turnOffLed(LEDS_COLOR color);
        void toggleLed(LEDS_COLOR color);


        // Funciones para la calibración
        pair<int, int> calibrate();
        bool saveCalibration();
        pair<int, int> loadCalibration();
    };

} /* namespace RoboCar */

#endif //ROBOCAR_ROBOCAR_H

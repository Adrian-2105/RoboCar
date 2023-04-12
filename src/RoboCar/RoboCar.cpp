#include "RoboCar/RoboCar.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include <unistd.h>

// Parámetros de configuración para la toma de medidas de distancias
#define NUM_DISTANCE_MEASURES           7

// Parámetros de configuración, obtenidos a mano, para el giro
#define TURN_SPEED_REFERENCE            55
#define TURN_ANGLE_REFERENCE            90
#define TURN_TIMEUMS_REFERENCE          500000
#define DELAY_TIMEUMS_TO_START_TURN     100000
#define CALCULATE_TURN_TIMEUMS(angle)   ((angle * TURN_TIMEUMS_REFERENCE / TURN_ANGLE_REFERENCE) + DELAY_TIMEUMS_TO_START_TURN)

// Pines para los LEDs
#define GREEN_LED_PIN_NUMBER            105
#define RED_LED_PIN_NUMBER              242

// Nombres de los ficheros para almacenar las calibraciones
#define LEFT_WHEEL_CALIBRATION_NAME     "leftWheel.calibration"
#define RIGHT_WHEEL_CALIBRATION_NAME    "rightWheel.calibration"

using namespace std;

namespace RoboCar{

    /**
     * @brief Inicializa sensores y pines necesarios para la configuración que hemos establecido
     * Nota: solo puede existir una misma instancia simultáneamente
     */
    RoboCar::RoboCar() {
        // Ruedas
        leftWheel = new WheelMotor(LEFT);
        rightWheel = new WheelMotor(RIGHT);
        // Sensor de ultrasonidos
        ultrasoundSensor = new UltrasoundSensor();
        // LEDS
        greenLed = new Led(GREEN_LED_PIN_NUMBER);
        redLed = new Led(RED_LED_PIN_NUMBER);

        // Parámetros por defecto
        speed = 0;
        maxSpeed = 0;
        minSpeed = 0;
    }

    /**
     * @brief Libera todos los recursos utilizados por el coche
     */
    RoboCar::~RoboCar() {
        delete leftWheel;
        delete rightWheel;
        delete ultrasoundSensor;
        delete greenLed;
        delete redLed;
    }

    /**
     * @brief El vehículo comienza a moverse hacia adelante a la velocidad configurada.
     * Se mueve de forma indefinida.
     */
    void RoboCar::goForward() {
        leftWheel->goForward();
        rightWheel->goForward();
    }

    /**
     * @brief El vehículo comienza a moverse hacia atrás a la velocidad configurada.
     * Se mueve de forma indefinida.
     */
    void RoboCar::goBackward() {
        leftWheel->goBackward();
        rightWheel->goBackward();
    }

    /**
     * @brief El vehículo comienza a moverse hacia la derecha a la velocidad configurada.
     * Se mueve de forma indefinida, rotando sobre su rueda derecha.
     */
    void RoboCar::goRight() {
        stop();
        leftWheel->goForward();
    }

    /**
     * @brief El vehículo comienza a moverse hacia la derecha hasta girar el ángulo indicado.
     * Rotando sobre su rueda derecha. Se parará por completo antes de girar.
     * @param angle Ángulo de giro (en grados sexagesimales [0-360])
     */
    void RoboCar::goRight(int angle) {
        // Se detiene el vehiculo por completo
        int lastSpeed = speed;
        stop();
        sleep(1);

        // Y se gira el ángulo indicado, apoyándose en las medidas tomadas previamente
        setSpeed(TURN_SPEED_REFERENCE);
        goRight();
        usleep(CALCULATE_TURN_TIMEUMS(angle));

        // Se vuelve a establecer la velocidad anterior y se detiene
        stop();
        setSpeed(lastSpeed);
    }

    /**
     * @brief El vehículo comienza a moverse hacia la izquierda a la velocidad configurada.
     * Se mueve de forma indefinida, rotando sobre su rueda izquierda.
     */
    void RoboCar::goLeft() {
        stop();
        rightWheel->goForward();
    }

    /**
     * @brief El vehículo comienza a moverse hacia la izquierda hasta girar el ángulo indicado.
     * Rotando sobre su rueda izquierda. Se parará por completo antes de girar.
     * @param angle Ángulo de giro (en grados sexagesimales [0-360])
     */
    void RoboCar::goLeft(int angle) {
        // Se detiene el vehiculo por completo
        int lastSpeed = speed;
        stop();
        sleep(1);

        // Y se gira el ángulo indicado, apoyándose en las medidas tomadas previamente
        setSpeed(TURN_SPEED_REFERENCE);
        goLeft();
        usleep(CALCULATE_TURN_TIMEUMS(angle));

        // Se vuelve a establecer la velocidad anterior y se detiene
        stop();
        setSpeed(lastSpeed);
    }

    /**
     * @brief El vehículo comienza a moverse hacia la derecha a la velocidad configurada.
     * Se mueve de forma indefinida, rotando sobre su propio eje.
     */
    void RoboCar::rotateRight() {
        stop();
        leftWheel->goForward();
        rightWheel->goBackward();
    }

    /**
     * @brief El vehículo comienza a moverse hacia la derecha hasta girar el ángulo indicado.
     * Rotando sobre su propio eje. Se parará por completo antes de girar.
     * @param angle Ángulo de giro (en grados sexagesimales [0-360])
     */
    void RoboCar::rotateRight(int angle) {
        // Se detiene el vehiculo por completo
        int lastSpeed = speed;
        stop();
        sleep(1);

        // Y se gira el ángulo indicado, apoyándose en las medidas tomadas previamente
        setSpeed(TURN_SPEED_REFERENCE);
        rotateRight();
        usleep(CALCULATE_TURN_TIMEUMS(angle));

        // Se vuelve a establecer la velocidad anterior y se detiene
        stop();
        setSpeed(lastSpeed);
    }

    /**
     * @brief El vehículo comienza a moverse hacia la izquierda a la velocidad configurada.
     * Se mueve de forma indefinida, rotando sobre su propio eje.
     */
    void RoboCar::rotateLeft() {
        stop();
        rightWheel->goForward();
        leftWheel->goBackward();
    }

    /**
     * @brief El vehículo comienza a moverse hacia la derecha hasta girar el ángulo indicado.
     * Rotando sobre su propio eje. Se parará por completo antes de girar.
     * @param angle Ángulo de giro (en grados sexagesimales [0-360])
     */
    void RoboCar::rotateLeft(int angle) {
        // Se detiene el vehiculo por completo
        int lastSpeed = speed;
        stop();
        sleep(1);

        // Y se gira el ángulo indicado, apoyándose en las medidas tomadas previamente
        setSpeed(TURN_SPEED_REFERENCE);
        rotateLeft();
        usleep(CALCULATE_TURN_TIMEUMS(angle));

        // Se vuelve a establecer la velocidad anterior y se detiene
        stop();
        setSpeed(lastSpeed);
    }

    /**
     * @brief Se detiene el movimiento del vehículo
     */
    void RoboCar::stop() {
        leftWheel->stop();
        rightWheel->stop();
    }

    /**
     * @brief Se establece la velocidad del coche a cada rueda
     * @param speed Velocidad a establecer. Este debe estar comprendido en el rango de velocidades mínima y máxima
     */
    void RoboCar::setSpeed(int speed) {
        this->speed = speed;
        leftWheel->setSpeed(speed);
        rightWheel->setSpeed(speed);
    }

    /**
 * @brief Se establece la velocidad máxima del coche a cada rueda
 */
    void RoboCar::setMaxSpeed() {
        setSpeed(maxSpeed);
    }

    /**
     * @brief Se establece la velocidad mínima del coche a cada rueda
     */
    void RoboCar::setMinSpeed() {
        setSpeed(minSpeed);
    }

    /**
     * @brief Se establece una velocidad intermedia del coche a cada rueda
     */
    void RoboCar::setMeanSpeed() {
        setSpeed((minSpeed + maxSpeed) / 2);
    }

    /**
     * @brief Retorna la velocidad especificada al vehículo (puede no coincidir con la real)
     * @return Velocidad a la que está configurado el vehículo
     */
    int RoboCar::getSpeed() const {
        return speed;
    }

    /**
     * @brief Retorna la velocidad máxima permitida del vehículo
     * @return Velocidad máxima permitida del vehículo
     */
    int RoboCar::getMaxSpeed() const {
        return maxSpeed;
    }

    /**
     * @brief Retorna la velocidad mínima permitida del vehículo
     * @return Velocidad mínima permitida del vehículo
     */
    int RoboCar::getMinSpeed() const {
        return minSpeed;
    }

    /**
     * @brief Actualiza la velocidad de las ruedas, para así regularlas y que estás vuelvan a alcanzar la velocidad
     * indicada inicialmente. Es recomendable que esta función sea llamada de forma periódica mientras el vehículo se
     * encuentra en movimiento.
     */
    void RoboCar::updateSpeed() {
        leftWheel->updateSpeed(speed);
        rightWheel->updateSpeed(speed);
    }

    /**
     * @brief Se toma una medida de la distancia, en CM, desde el vehículo al siguiente obstáculo que se encuentre
     * en frente de él. Debido a la variación en las medidas que ofrece el sensor, se realizan varias medidas y se
     * seleccionan las que, estadísticamente, se consideran más correctas.
     * @return Distancia, en CM, a la que se encuentra el pŕoximo obstáculo
     */
    float RoboCar::getDistance() {
        // Tomamos NUM_DISTANCE_MEASURES (p.e: 11), comprobando que no se estén tomando medidas erróneas
        std::vector<float> distances;
        for (int i = 0; i < NUM_DISTANCE_MEASURES; i++) {
            float distance = ultrasoundSensor->getDistance();
            if (distance != -1) {
                distances.push_back(distance);
            }
        }
        // En caso de que hayan sido todas erróneas, devolvemos error
        if (distances.empty())
            return -1;

        // Ordenamos las medidas tomadas para poder calcular la mediana
        std::sort(distances.begin(), distances.end());
        float median = distances[distances.size() / 2];

        // Calculamos la desviación típica de las medidas tomadas
        float sum = std::accumulate(distances.begin(), distances.end(), 0.0f);
        float mean = sum / distances.size();
        float sqSum = std::inner_product(distances.begin(), distances.end(), distances.begin(), 0.0);
        float stDev = std::sqrt(sqSum / distances.size() - mean * mean);

        // Calculamos la distancia como la media de los valores que entre en el rango [median - stDev >= X <= median + stDev]
        float total = 0.0f;
        int elementsForMean = 0;
        for (float distance: distances) {
            if (distance >= median - stDev && distance <= median + stDev) {
                total += distance;
                elementsForMean++;
            }
        }
        return total / elementsForMean;
    }

    /**
     * @brief Enciende el LED del color especificado
     * @param color Color del LED a encender
     */
    void RoboCar::turnOnLed(LEDS_COLOR color) {
        switch (color) {
            case GREEN:
                greenLed->turnOn();
                break;
            case RED:
                redLed->turnOn();
                break;
            default:
                std::cerr << "Color de LED invalido" << std::endl;
                break;
        }
    }

    /**
     * @brief Apaga el LED del color especificado
     * @param color Color del LED a apagar
     */
    void RoboCar::turnOffLed(LEDS_COLOR color) {
        switch (color) {
            case GREEN:
                greenLed->turnOff();
                break;
            case RED:
                redLed->turnOff();
                break;
            default:
                std::cerr << "Color de LED invalido" << std::endl;
                break;
        }
    }

    /**
     * @brief Alterna el estado del LED del color especificado
     * @param color Color del LED a alternar
     */
    void RoboCar::toggleLed(LEDS_COLOR color) {
        switch (color) {
            case GREEN:
                greenLed->toggle();
                break;
            case RED:
                redLed->toggle();
                break;
            default:
                std::cerr << "Color de LED invalido" << std::endl;
                break;
        }
    }

    /**
     * @brief Se calibra el coche, poníendose a girar ambas ruedas.
     * Este proceso dura entre 20 y 30 segundos en realizarse
     * @return Valores mínimo y máximo de movimiento que permite el coche
     */
    std::pair<int, int> RoboCar::calibrate() {
        pair<int, int> left = leftWheel->calibrate();
        pair<int, int> right = rightWheel->calibrate();
        minSpeed = max(left.minimum, right.minimum);
        maxSpeed = min(left.maximum, right.maximum);
        return {minSpeed, maxSpeed};
    }

    /**
     * @brief Guarda la calibración realizada para el coche
     * @return true si se ha guardado correctamente, false si ha ocurrido algún error
     */
    bool RoboCar::saveCalibration() {
        return leftWheel->saveCalibration(LEFT_WHEEL_CALIBRATION_NAME) &&
               rightWheel->saveCalibration(RIGHT_WHEEL_CALIBRATION_NAME);
    }

    /**
     * @brief Carga la calibración realizada para el coche
     * @return pair<minimo, maximo> con las velocidades cargadas, <0, 0> en caso de error
     */
    pair<int, int> RoboCar::loadCalibration() {
        pair<int, int> left = leftWheel->loadCalibration(LEFT_WHEEL_CALIBRATION_NAME);
        pair<int, int> right = rightWheel->loadCalibration(RIGHT_WHEEL_CALIBRATION_NAME);
        if (left.minimum != 0 && left.maximum != 0 && right.minimum != 0 && right.maximum != 0) {
            minSpeed = max(left.minimum, right.minimum);
            maxSpeed = min(left.maximum, right.maximum);
            return {minSpeed, maxSpeed};
        } else {
            return {0, 0};
        }
    }

}
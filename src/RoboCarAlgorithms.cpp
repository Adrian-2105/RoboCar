#include "RoboCarAlgorithms.h"
#include <iostream>
#include <unistd.h>

// Parámetros de configuración de espera para los algoritmos
#define DEFAULT_DELAY_TIMEUMS       500000
#define DELAY_BETWEEN_ITERATIONS    100000

namespace RoboCarAlgorithms {

    /**
     * @brief El coche comienza a moverse en linea recta detectando obstáculos. En caso de que vaya a chocar,
     * se detiene y girar hacia los lados. En caso de que no pueda girar, retrocederá marcha atrás.
     * @param car RoboCar 
     * @param time Tiempo total de funcionamiento en segundos
     */
    void simpleMode(RoboCar::RoboCar *car, int time, int limitDistance, bool maxSpeed) {
        std::cout << "Iniciando modo de movimiento \"simple\"" << std::endl;

        // Se establece la velocidad y el coche empieza a moverse
        if (maxSpeed)
            car->setMaxSpeed();
        else
            car->setMinSpeed();
        std::cout << "RoboCar se movera a una velocidad de " << car->getSpeed() << std::endl;
        car->goForward();
        car->turnOnLed(RoboCar::GREEN);

        // Inicialización de variables y temporizadores
        long long totalTime = CLOCKS_PER_SEC * time;
        clock_t start = clock();
        clock_t actual = start;

        // Bucle principal de funcionamiento
        while ((actual - start) < totalTime) {
            // Se toma una medida de la distancia y se actúa en funcioón de ella
            float distance = car->getDistance();

            if (distance < limitDistance || distance == -1) {
                car->turnOffLed(RoboCar::GREEN);
                car->turnOnLed(RoboCar::RED);

                // El coche ha detectado un obstáculo y debe evitarlo
                int attempts = 0;
                while (distance < limitDistance || distance == -1) {
                    std::cout << "Obstaculo detectado a " << distance << " CM" << std::endl;
                    switch (attempts) {
                        case 0: // Comprobar si se puede avanzar a la derecha
                            std::cout << "Girando a la derecha..." << std::endl;
                            car->rotateRight(90);
                            break;
                        case 1:
                            std::cout << "Girando a la izquierda..." << std::endl;
                            car->rotateLeft(180);
                            break;
                        default: // Si no se puede girar a ningún lado, se vuelve a la posición inicial y se retrocede
                            std::cout << "Camino no encontrado. Retrocediendo..." << std::endl;
                            car->rotateRight(90),
                            car->goBackward();
                            usleep(DEFAULT_DELAY_TIMEUMS);
                            car->rotateRight(90);
                            attempts = 0;
                            break;
                    }

                    // Se toma una nueva medida para comprobar si
                    distance = car->getDistance();
                    attempts++;
                }

                std::cout << "Obstaculo evitado. Continuando..." << std::endl;
                car->stop();
                usleep(DEFAULT_DELAY_TIMEUMS);
                car->goForward();
                car->turnOffLed(RoboCar::RED);
                car->turnOnLed(RoboCar::GREEN);

            } else {
                // Si el coche no va a chocarse entonces actualizamos su velocidad para mantenerla a la especificada
                car->updateSpeed();
            }

            // Control de tiempo entre iteraciones
            usleep(DELAY_BETWEEN_ITERATIONS);
            actual = clock();
        }

        // Se termina la ejecución y se detiene el vehículo
        car->stop();
        car->turnOffLed(RoboCar::GREEN);
        car->turnOffLed(RoboCar::RED);
    }

    /**
     * @brief Modo de funcionamiento de RoboCar en el que este gira haciendo uso de sus dos ruedas
     * a máxima velocidad durante el tiempo estipulado. Alterna el giro entre la izquierda y la derecha.
     * @param car RoboCar
     * @param time Tiempo total de funcionamiento en segundos
     */
    void twisterMode(RoboCar::RoboCar *car, int time) {
        std::cout << "Iniciando modo de movimiento \"tornado\"" << std::endl;
        int turnTime = (time - 1) / 2;

        // Se establece la máxima velocidad
        car->setMaxSpeed();

        // Se comienza la primera fase de giro a la derecha
        car->turnOnLed(RoboCar::GREEN);
        car->rotateRight();
        sleep(turnTime);
        car->stop();
        sleep(1);

        // Se realiza la segunda fase de giro a la izquierda
        car->turnOffLed(RoboCar::GREEN);
        car->turnOnLed(RoboCar::RED);
        car->rotateLeft();
        sleep(turnTime);

        // Y finalmente se detiene el vehiculo
        car->stop();
        car->turnOffLed(RoboCar::RED);
    }

    /**
     * @brief El coche comienza a seguir un circuito especificado en un fichero. En este se especifican en qué sentido y ángulo
     * son las curvas. El coche siempre va en línea recta y, al detectar un obstáculo, tomará la siguiente decisión en la lista.
     * También realiza pequeños ajustes para corregir posibles errores y mantenerse en la trazada.
     * @param car RoboCar
     * @param circuitFilename Nombre del fichero que contiene la especificación del circuito
     */
    void circuitMode(RoboCar::RoboCar *car, int time, int limitDistance, const string &circuitFilename) {
        // Comprobación de parámetros
        if (circuitFilename.empty()) {
            std::cerr << "Se tiene que establecer un fichero con la configuracion del circuito" << std::endl;
            return;
        }
        std::ifstream circuitFile(circuitFilename);
        if (circuitFile.bad()) {
            std::cerr << "El fichero de configuracion del circuito < " << circuitFilename << " > no es valido" << std::endl;
            return;
        }

        // Lectura y parseo del fichero de configuración
        std::vector<std::pair<char, int>> curves;
        char direction;
        int angle;
        while (circuitFile >> direction >> angle){
            if (direction != 'l' && direction != 'L' && direction != 'r' && direction != 'R') {
                std::cerr << "Error: la direcction " << direction << " es invalida" << std::endl;
                return;
            }
            if (angle < 0 || angle > 360) {
                std::cerr << "Error: el angulo " << angle << " es invalido" << std::endl;
                return;
            }
            curves.push_back({direction, angle});
        }
        if (curves.empty()) {
            std::cerr << "El fichero de configuracion del circuito < " << circuitFilename << " > esta vacio" << std::endl;
            return;
        }

        long long totalTime = CLOCKS_PER_SEC * time;

        // Algoritmo
        std::cout << "Iniciando modo de movimiento \"circuito\"" << std::endl;
        car->goForward();
        car->setMeanSpeed();
        car->turnOnLed(RoboCar::GREEN);

        int decision = 0;
        char nextDirection = curves[decision].first;
        int nextAngle = curves[decision].second;

        clock_t start = clock();
        clock_t actual = start;
        while ((actual - start) < totalTime) {
            float distance = car->getDistance();

            if (distance < limitDistance || distance == -1) {
                car->turnOffLed(RoboCar::GREEN);
                car->turnOnLed(RoboCar::RED);

                // Si se detecta un obstáculo entonces se toma la siguiente decisión
                if (nextDirection == 'l' || nextDirection == 'L') {
                    std::cout << "Girando a la izquierda " << nextAngle << " grados..." << std::endl;
                    car->rotateLeft(nextAngle);
                } else { // (nextDirection == 'r' || nextDirection == 'R')
                    std::cout << "Girando a la derecha " << nextAngle << " grados..." << std::endl;
                    car->rotateRight(nextAngle);
                }
                usleep(DEFAULT_DELAY_TIMEUMS);

                // Actualizamos la siguiente decisión
                decision = (decision + 1) % curves.size();
                nextDirection = curves[decision].first;
                nextAngle = curves[decision].second;

                car->turnOnLed(RoboCar::GREEN);
                car->turnOffLed(RoboCar::RED);
            } else {
                // Si el coche no va a chocarse entonces actualizamos su velocidad para mantenerla a la especificada
                car->updateSpeed();
            }

            // Control de tiempo entre iteraciones
            usleep(DELAY_BETWEEN_ITERATIONS);
            car->setMeanSpeed();
            car->goForward();
            actual = clock();
        }

        // Y finalmente se detiene el vehiculo
        car->stop();
        car->turnOffLed(RoboCar::GREEN);
        car->turnOffLed(RoboCar::RED);
    }

}
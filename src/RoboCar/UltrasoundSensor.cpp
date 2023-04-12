#include "RoboCar/UltrasoundSensor.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

// Pins GPIO preconfigurados para su uso
#define ULTRASOUND_TRIGGER_PIN  234
#define ULTRASOUND_ECHO_PIN     209

// Velocidad del sonido
#define CM_PER_SECOND           34300.0f

// Parámetros de configuración para las mediciones
#define UMS_INTERVAL_TIME       5
#define MAX_ATTEMPTS            3
#define THRESHOLD_WRONG_VALUE   2000

namespace RoboCar {

    /**
     * @brief Inicializa los pines y configura el sensor
     * Nota: solo puede existir una instancia de sensor simultáneamente
     */
    UltrasoundSensor::UltrasoundSensor() {
        triggerPin = new PinsLib::GPIO(ULTRASOUND_TRIGGER_PIN);
        triggerPin->setDirection(PinsLib::OUTPUT);
        echoPin = new PinsLib::GPIO(ULTRASOUND_ECHO_PIN);
        echoPin->setDirection(PinsLib::INPUT);
    }

    /**
     * @brief Elimina la exportación de los pines
     */
    UltrasoundSensor::~UltrasoundSensor() {
        delete triggerPin;
        delete echoPin;
    }

    /**
     * @brief Obtiene la distancia, en centímetros, que mide el sensor hasta el siguiente obstáculo
     * @return float Distancia en CM. -1 en caso de medida errónea
     */
    float UltrasoundSensor::getDistance() {
        // Reiniciamos el pin de Trigger
        triggerPin->setValue(PinsLib::LOW);
        usleep(UMS_INTERVAL_TIME);

        // Emitimos una onda con el trigger
        triggerPin->setValue(PinsLib::HIGH);
        usleep(UMS_INTERVAL_TIME);
        triggerPin->setValue(PinsLib::LOW);

        // Esperar hasta obtener un 1 en el echo (o que el valor ya sea 0, en el cual será error)
        int cont = 0;
        while(echoPin->getValue() != 1 && cont < MAX_ATTEMPTS) {
            cont++;
        }
        if (cont >= MAX_ATTEMPTS)
            return -1;

        // Tomamos una marca de tiempo
        clock_t startTime = clock();

        // Esperar mientras el valor pase a ser 0 (flanco descendente)
        while(echoPin->getValue() != 0);

        // Tomamos otra marca de tiempo final
        clock_t stopTime = clock();

        // Calculamos la distancia (en CM)
        float distance = (((float) (stopTime - startTime) / (float) CLOCKS_PER_SEC) * (float) CM_PER_SECOND) / 2.0f;
        // Y comprobamos si no se ha superado el umbral de error antes de retornarlo
        return (distance < THRESHOLD_WRONG_VALUE) ? distance : -1;
    }

} /* namespace RoboCar */

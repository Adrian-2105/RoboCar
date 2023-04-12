#include "RoboCar/Led.h"

namespace RoboCar {

    /**
     * @brief Constructor para un LED. Inicializa y exporta el pin
     * @param number Número de pin donde está conectado el LED
     */
    Led::Led(int number) {
        ledPin = new PinsLib::GPIO(number);
        ledPin->setDirection(PinsLib::OUTPUT);
        turnOff();
    }

    /**
     * @brief Elimina el LED y su exportación
     */
    Led::~Led() {
        delete ledPin;
    }

    /**
     * @brief Retorna si el LED está encendido o no
     * @return true si está encendido, false en caso contrario
     */
    bool Led::isOn() const {
        return on;
    }

    /**
     * @brief Enciende el LED
     */
    void Led::turnOn() {
        on = true;
        ledPin->setValue(PinsLib::HIGH);
    }

    /**
     * @brief Apaga el LED
     */
    void Led::turnOff() {
        on = false;
        ledPin->setValue(PinsLib::LOW);
    }

    /**
     * @brief Alterna el estado del LED
     */
    void Led::toggle() {
        if (on)
            turnOff();
        else
            turnOn();
    }

}

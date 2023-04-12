#include "RoboCar/WheelMotor.h"
#include <iostream>
#include <time.h>
#include <unistd.h>

// Pins GPIO y PWM preconfigurados para su uso
#define LEFT_WHEEL_FORWARD_PIN      178
#define LEFT_WHEEL_BACKWARD_PIN     164
#define LEFT_WHEEL_ENCODER_PIN      208
#define LEFT_WHEEL_PWM_PIN          1

#define RIGHT_WHEEL_FORWARD_PIN     166
#define RIGHT_WHEEL_BACKWARD_PIN    165
#define RIGHT_WHEEL_ENCODER_PIN     177
#define RIGHT_WHEEL_PWM_PIN         0

// Parámetros para la configuración del periodo y duty cycle
#define PERIOD                      4000
#define DEFAULT_DUTYCYCLE           0

// Parámetros de configuración para la toma de medidas
#define MEASURES_FOR_SPEED          5
#define MAX_ATTEMPTS_TO_READ        250
#define CALIBRATION_WAIT_DELAY      200000

// Constante de PID proporcional (K) para la modificación del duty cycle
#define DUTYCYCLE_CONSTANT          5

namespace RoboCar {

    /**
     * @brief Constructor que incializa los distintos pines utilizados por cada una de las ruedas (LEFT, RIGHT)
     * Nota: solo se puede crear una instancia de cada tipo
     * @param wheel Rueda que se quiere instanciar
     */
    WheelMotor::WheelMotor(Wheel wheel){
        int forwardPinNumber, backwardPinNumber, encoderPinNumber, speedPinNumber;
        switch (wheel) {
            case LEFT:
                forwardPinNumber = LEFT_WHEEL_FORWARD_PIN;
                backwardPinNumber = LEFT_WHEEL_BACKWARD_PIN;
                encoderPinNumber = LEFT_WHEEL_ENCODER_PIN;
                speedPinNumber = LEFT_WHEEL_PWM_PIN;
                break;
            case RIGHT:
                forwardPinNumber = RIGHT_WHEEL_FORWARD_PIN;
                backwardPinNumber = RIGHT_WHEEL_BACKWARD_PIN;
                encoderPinNumber = RIGHT_WHEEL_ENCODER_PIN;
                speedPinNumber = RIGHT_WHEEL_PWM_PIN;
                break;
            default: // Gestión de errores
                std::cerr << "El numero de rueda no es valido" << std::endl;
                exit(1);
        }

        // Se crean y se configuran los pins GPIO
        forwardPin = new PinsLib::GPIO(forwardPinNumber);
        forwardPin->setDirection(PinsLib::OUTPUT);
        backwardPin = new PinsLib::GPIO(backwardPinNumber);
        backwardPin->setDirection(PinsLib::OUTPUT);
        encoderPin = new PinsLib::GPIO(encoderPinNumber);
        encoderPin->setDirection(PinsLib::INPUT);

        // Se crean y se configuran los pins PWM
        speedPin = new PinsLib::PWM(speedPinNumber);
        speedPin->setPeriod(PERIOD);

        // Inicialización de parámetros generales
        moving = false;
        calibrated = false;
        minSpeed = 0;
        maxSpeed = 0;
        dutyCycle = DEFAULT_DUTYCYCLE;

        // Establecimiento de la velocidad por defecto
        setDutyCycle(dutyCycle);
    }

    /**
     * @brief Elimina la rueda y libera sus respectivos pines
     */
    WheelMotor::~WheelMotor(){
        delete forwardPin;
        delete backwardPin;
        delete encoderPin;
        delete speedPin;
    }

    /**
     * @brief Hace a la rueda avanzar hacia adelante a la velocidad establecida
     */
    void WheelMotor::goForward() {
        moving = true;
        // Activamos los pines correspondientes para ir hacia adelante
        backwardPin->setValue(PinsLib::LOW);
        forwardPin->setValue(PinsLib::HIGH);
        // Y se habilita el PWM para permitir el movimiento
        speedPin->setEnable(PinsLib::HIGH);
    }

    /**
     * @brief Hace a la rueda avanzar hacia atrás a la velocidad establecida
     */
    void WheelMotor::goBackward() {
        moving = true;
        // Activamos los pines correspondientes para ir marcha atrás
        forwardPin->setValue(PinsLib::LOW);
        backwardPin->setValue(PinsLib::HIGH);
        // Y se habilita el PWM para permitir el movimiento
        speedPin->setEnable(PinsLib::HIGH);
    }

    /**
     * @brief Detiene la rueda
     */
    void WheelMotor::stop() {
        moving = false;
        // Se deshabilita el PWM para impedir el movimiento
        speedPin->setEnable(PinsLib::LOW);
        // Y se desactivan los pines
        forwardPin->setValue(PinsLib::LOW);
        backwardPin->setValue(PinsLib::LOW);

    }

    /**
     * @brief Función privada que altera el DutyCycle, alterando así también la velocidad
     * @param _dutyCycle Valor de Duty Cycle utilizado. Debe ceñirse al invervalo de valores posibles [0, PERIOD]
     */
    void WheelMotor::setDutyCycle(int _dutyCycle) {
        if (_dutyCycle < 0 || _dutyCycle > PERIOD) {
            std::cerr << "Valor de Duty Cycle invalido: " << _dutyCycle << std::endl;
            return;
        }
        dutyCycle = _dutyCycle;
        speedPin->setDutyCycle(dutyCycle);
    }

    /**
     * @brief Actualiza la velocidad de la rueda haciendo uso de la tabla de calibraciones.
     * Para ello la rueda debe haber estado previamente calibrada
     * @param speed Velocidad a establecer. Debe entrar dentro del rango de valores máximo y mínimo
     * @return true si se ha podido establecer la velocidad, false en caso contrario
     */
    bool WheelMotor::setSpeed(int speed) {
        if (!calibrated) {
            std::cerr << "La rueda no esta calibrada, no se pudo establecer la velocidad" << std::endl;
            return false;
        }

        if (!(speed >= minSpeed && speed <= maxSpeed)) {
            std::cerr << "La velocidad " << speed << " no es valida. Min=" << minSpeed << " Max=" << maxSpeed << std::endl;
            return false;
        }

        // Se recorre el array con los valores de calibración y se utiliza el más adecuado
        for (int i = 0; i < speeds.size() - 1; i++) {
            if (speeds[i].second <= speed && speeds[i + 1].second >= speed) {
                setDutyCycle(speeds[i].first);
                return true;
            }
        }

        std::cerr << "Error: ocurrio un error inesperado al establecer una velocidad de " << speed << std::endl;
        return false;
    }

    /**
     * @brief Obtiene cuál es la velocidad de movimiento actual de la rueda.
     * Esta es muy probable que difiera con la que se ha establecido previamente con setSpeed(),
     * por lo que es recomendable actualizar la velocidad con updateSpeed() si se detecta que
     * ha variado
     * @return Valor de la velocidad actual (tacos/s), 0 en caso de que se encuentre quieta o
     * haya sucedido algún error
     */
    int WheelMotor::getCurrentSpeed() {
        if (!moving)
            return 0;

        // Se inicia el temporizador para la medición
        clock_t startTime = clock();
        int cont = 0;
        // Se toma la medida del tiempo en recorrer MEASURES_FOR_SPEED tacos
        // Recorrer un taco se considera como una alteración en el encoder (tacómetro) entre 0 y 1
        for (int i = 0; i < MEASURES_FOR_SPEED; i++) {
            // Se realizan un máximo de lectura de cada pin y en caso de exceder un máximo de lecturas se considera que
            // la rueda está parada (aunque se le haya indicado moverse, no tiene potencia suficiente como para hacerlo)
            while(encoderPin->getValue() != 1 && cont++ < MAX_ATTEMPTS_TO_READ);
            if (cont >= MAX_ATTEMPTS_TO_READ) return 0;
            else cont = 0;

            while(encoderPin->getValue() != 0 && cont++ < MAX_ATTEMPTS_TO_READ);
            if (cont >= MAX_ATTEMPTS_TO_READ) return 0;
            else cont = 0;
        }
        clock_t stopTime = clock();

        // Con la diferencia de tiempo de las medidas, se calcula la velocidad actual de la rueda
        return CLOCKS_PER_SEC * MEASURES_FOR_SPEED / (float) (stopTime - startTime);
    }

    /**
     * @brief Actualiza de nuevo la velocidad de la rueda en base a una velocidad de referencia.
     * Para ello hace uso de un PID proporcional básico
     * @param referenceSpeed Velocidad de referencia sobre la que regular la velocidad.
     * Generamente será el mismo valor que anteriormente se le haya establecido en setSpeed()
     */
    void WheelMotor::updateSpeed(int referenceSpeed) {
        if (!moving)
            return;

        // PID proporcional para la regulación de la velocidad, regulando para ello el duty cycle
        int currentSpeed = getCurrentSpeed();
        int dutyCycle_change = (referenceSpeed - currentSpeed) * DUTYCYCLE_CONSTANT;
        dutyCycle += dutyCycle_change;
        if (dutyCycle > PERIOD)
            dutyCycle = PERIOD;
        setDutyCycle(dutyCycle);
    }

    /**
     * @brief Inicia un proceso de calibración con el cuál la rueda comenzará a moverse a distintas velocidades.
     * Este proceso dura entre 10 y 20 segundos
     * @return Valores mínimos y máximo de velocidad tras la calibración
     */
    std::pair<int, int> WheelMotor::calibrate() {
        // Inicialización de parámetros
        speeds.clear();
        minSpeed = INT32_MAX;
        maxSpeed = 0;
        setDutyCycle(DEFAULT_DUTYCYCLE);

        // Se detiene el coche y se espera a que esté quieto
        stop();
        usleep(CALIBRATION_WAIT_DELAY);

        // Realizamos el calibrado para 40 Duty Cycles distintos
        goForward();
        for (int dutyCycle = 0; dutyCycle <= PERIOD; dutyCycle += 100) {
            // Establecemos la nueva velocidad
            setDutyCycle(dutyCycle);
            // Dejamos un margen de espera hasta que se ponga la nueva velocidad
            usleep(CALIBRATION_WAIT_DELAY);

            // Tomamos varias medidas de velocidad y realizamos la media
            int speed = 0;
            for (int i = 0; i < MEASURES_FOR_SPEED; i++) {
                speed += getCurrentSpeed();
            }
            speed /= MEASURES_FOR_SPEED;

            // Añadimos el par DutyCycle-speed a la tabla de velocidades (siempre que esté en movimiento)
            if (speed > 0) {
                speeds.push_back({dutyCycle, speed});
                minSpeed = std::min(minSpeed, speed);
                maxSpeed = std::max(maxSpeed, speed);
            }
        }

        // Terminamos y retornamos los resultados de la calibración
        stop();
        calibrated = true;
        return {minSpeed, maxSpeed};
    }

    /**
     * @brief Guarda la calibración realizada en un fichero
     * @param filename Nombre del fichero donde se quiere guardar (ruta relativa)
     * @return true si se pudo guardar la configuración, false en caso contrario
     */
    bool WheelMotor::saveCalibration(std::string filename) {
        // Apertura de fichero
        std::ofstream out(filename);
        if (out.bad()) {
            std::cerr << "No se pudo abrir el fichero " << filename << " para escritura" << std::endl;
            return false;
        }

        // Escritura en fichero
        for (std::pair<int, int> s : speeds)
            out << s.first << " " << s.second << std::endl;
        out.close();

        return true;
    }

    /**
     * @brief Carga la calibración almacenada en un fichero
     * @param filename Nombre del fichero que se quiere cargar (ruta relativa)
     * @return pair<minimo, maximo> con las velocidades cargadas, <0, 0> en caso de error
     */
    pair<int, int> WheelMotor::loadCalibration(std::string filename) {
        // Apertura de fichero
        std::ifstream in(filename);
        if (in.bad()) {
            std::cerr << "No se pudo abrir el fichero " << filename << " para lectura" << std::endl;
            return {0, 0};
        }

        // Carga de valores desde fichero
        int duty_cycle, speed;
        speeds.clear();
        minSpeed = INT32_MAX;
        maxSpeed = 0;
        while (in >> duty_cycle >> speed) {
            speeds.push_back({duty_cycle, speed});
            minSpeed = std::min(minSpeed, speed);
            maxSpeed = std::max(maxSpeed, speed);
        }
        in.close();

        calibrated = true;
        return {minSpeed, maxSpeed};
    }

} /* namespace RoboCar */
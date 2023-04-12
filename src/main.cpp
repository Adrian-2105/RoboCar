#include <iostream>
#include <getopt.h>
#include "RoboCarAlgorithms.h"

// Valores por defecto para los parámetros
#define DEFAULT_TIME                30
#define DEFAULT_LIMIT_DISTANCE      35
#define DEFAULT_MAXSPEED_ENABLED    false

void printHelp(char **argv) {
    std::cout << "USO: " << argv[0] << " [OPCIONES]" << std::endl;
    std::cout << std::endl;
    std::cout << "# Opciones: " << std::endl;
    std::cout << std::endl;
    std::cout << "  -c, --calibrate" << std::endl;
    std::cout << "      Se realiza una ejecucion de calibracion. La calibracion es guardada en un fichero" << std::endl;
    std::cout << std::endl;
    std::cout << "  -m, --mode <OPCION>" << std::endl;
    std::cout << "    (necesario que el coche se encuentre previamente calibrado)" << std::endl;
    std::cout << "    - simple : movimiento aleatorio evitando obstaculos" << std::endl;
    std::cout << "    - twister , tornado : movimiento rotatorio" << std::endl;
    std::cout << "    - circuit : movimiento siguiendo el circuito que se indique en un fichero" << std::endl;
    std::cout << std::endl;
    std::cout << "  -t, --time <SEGUNDOS>" << std::endl;
    std::cout << "    (opcional, por defecto = 30)" << std::endl;
    std::cout << "    Tiempo de funcionamiento del vehiculo" << std::endl;
    std::cout << std::endl;
    std::cout << "  -d, --distance <CENTIMETROS>" << std::endl;
    std::cout << "    (opcional, por defecto = 35)" << std::endl;
    std::cout << "    Distancia a la que se detectan obstaculos" << std::endl;
    std::cout << std::endl;
    std::cout << "  -s, --maxSpeed" << std::endl;
    std::cout << "    Si se habilita, el vehiculo se movera a maxima velocidad" << std::endl;
    std::cout << std::endl;
    std::cout << "  -k, --circuit <NOMBRE_FICHERO>" << std::endl;
    std::cout << "    (obligatorio si --mode circuit)" << std::endl;
    std::cout << "    Especifica las curvas que tiene el circuito" << std::endl;
    std::cout << std::endl;
    std::cout << "  -h, --help" << std::endl;
    std::cout << "    Muestra este menu de ayuda" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char **argv) {

    /*** Lectura y parseo de argumentos ***/
    bool calibrate = false;
    std::string mode;
    int time = DEFAULT_TIME;
    int limitDistance = DEFAULT_LIMIT_DISTANCE;
    bool maxSpeed = DEFAULT_MAXSPEED_ENABLED;
    std::string circuit;

    struct option long_options[] = {
            {"calibrate", no_argument,       nullptr, 'c'},
            {"mode",      required_argument, nullptr, 'm'},
            {"time",      required_argument, nullptr, 't'},
            {"distance",  required_argument, nullptr, 'd'},
            {"maxSpeed",  no_argument,       nullptr, 's'},
            {"circuit",   required_argument, nullptr, 'k'},
            {"help",      no_argument,       nullptr, 'h'},
            {nullptr,     0,                 nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'c':
                calibrate = true;
                break;
            case 'm':
                mode = optarg;
                break;
            case 't':
                time = stoi(optarg);
                break;
            case 'd':
                limitDistance = stoi(optarg);
                break;
            case 's':
                maxSpeed = true;
                break;
            case 'k':
                circuit = optarg;
                break;
            case 'h':
            default:
                printHelp(argv);
                exit(EXIT_FAILURE);
        }
    }

    /*** Gestión de la calibración de RoboCar ***/
    auto *robocar = new RoboCar::RoboCar();

    if (calibrate) {
        robocar->calibrate();
        robocar->saveCalibration();
        exit(EXIT_SUCCESS);
    } else {
        std::pair<int, int> results = robocar->loadCalibration();
        if (results.minimum == 0 && results.maximum == 0) {
            exit(EXIT_FAILURE);
        }
    }

    /*** Ejecución del algoritmo en función del modo ***/
    if (mode.empty()) {
        printHelp(argv);
        exit(EXIT_FAILURE);
    } else if (mode == "simple") {
        RoboCarAlgorithms::simpleMode(robocar, time, limitDistance, maxSpeed);
    } else if (mode == "twister" || mode == "tornado") {
        RoboCarAlgorithms::twisterMode(robocar, time);
    } else if (mode == "circuit") {
        RoboCarAlgorithms::circuitMode(robocar, time, limitDistance, circuit);
    } else {
        std::cerr << "No se reconoce el modo << " << mode << std::endl;
        printHelp(argv);
        exit(EXIT_FAILURE);
    }

    delete robocar;

    return EXIT_SUCCESS;
}
#include "PinsLib/Pins.h"
#include <sstream>
#include <unistd.h>

using namespace std;

namespace PinsLib {

    Pins::Pins(int number, string exportPath) {
        this->number = number;
        this->exportPath = exportPath;

        // Exportamos el pin creado
        this->exportPin();
        
        // need to give Linux time to set up the sysfs structure
        usleep(250000); // 250ms delay
    }

    Pins::~Pins() {
        this->unexportPin();
    }

    int Pins::write(string path, string filename, string value) {
        ofstream fs;
        fs.open((path + filename).c_str());
        if (!fs.is_open()) {
            perror("PinsLib: write failed to open file ");
            return -1;
        }
        fs << value;
        fs.close();
        return 0;
    }

    int Pins::write(string path, string filename, int value) {
        stringstream s;
        s << value;
        return this->write(path, filename, s.str());
    }

    int Pins::write(string filename, string value) {
        return this->write(path, filename, value);
    }

    int Pins::write(string filename, int value) {
        return this->write(path, filename, value);
    }

    string Pins::read(string path, string filename) {
        ifstream fs;
        fs.open((path + filename).c_str());
        if (!fs.is_open()) {
            perror("PinsLib: read failed to open file ");
        }
        string input;
        getline(fs, input);
        fs.close();
        return input;
    }

    string Pins::read(string filename) {
        return read(path, filename);
    }

    int Pins::exportPin() {
        // Desexportamos previamente el pin para evitar posibles fallos
        this->write(exportPath, "unexport", this->number);
        return this->write(exportPath, "export", this->number);
    }

    int Pins::unexportPin() {
        return this->write(exportPath, "unexport", this->number);
    }

}
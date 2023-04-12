#ifndef PINS_H_
#define PINS_H_

#include <string>
#include <fstream>

using std::string;
using std::ofstream;

namespace PinsLib {

    class Pins {
        friend class GPIO;
        friend class PWM;

    private:
        int number;
        string name, path, exportPath;
        ofstream stream;

    public:
        // Constructor general que también exportará el pin indicado
        Pins(int number, string exportPath);

        // Destructor. También eliminará la exportación del pin
        ~Pins();

        int getNumber() { return number; }

    private:
        // Operaciones de escritura sobre los ficheros de manejo del pin
        int write(string filename, string value);
        int write(string filename, int value);
        int write(string path, string filename, string value);
        int write(string path, string filename, int value);

        string read(string filename);
        string read(string path, string filename);

        // Exporta el pin para poder ser utilizado
        int exportPin();

        // Elimina la exportación del pin
        int unexportPin();

    };

} /* namespace PinsLib */

#endif /* PINS_H_ */

#ifndef ROBOCAR_ROBOCARALGORITHMS_H
#define ROBOCAR_ROBOCARALGORITHMS_H

#include "RoboCar/RoboCar.h"

using namespace std;

namespace RoboCarAlgorithms {

    void simpleMode(RoboCar::RoboCar *car, int time, int limitDistance, bool maxSpeed);

    void twisterMode(RoboCar::RoboCar *car, int time);

    void circuitMode(RoboCar::RoboCar *car, int time, int limitDistance, const string& circuitFilename);

} /* namespace RoboCarAlgorithms */



#endif //ROBOCAR_ROBOCARALGORITHMS_H

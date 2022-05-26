#ifndef HYPERCUBEFUNCTION_H
#define HYPERCUBEFUNCTION_H

#include <unordered_map>
#include "../../Vector-Modules/Point/Point.h"
#include "../Random-Function/RandomFunction.h"


/* Class that acts as the h_i() function in the context of the Hypercube algorithm */

class HypercubeFunction {

private:
    /* Each h_i() function is associated with a unique f_i() function */
    RandomFunction* randomFunction;
    Vector *unitVector;
    float shift;
    unsigned int window;
    /* Data structure used to store a just/previously calculated h_i() value and its appropriate mapping to 0 or 1 */
    std::unordered_map<long long int, std::string> buckets;

public:
    HypercubeFunction(unsigned int, double, unsigned int);
    ~HypercubeFunction();
    std::string hash(const Point &);

};


#endif

#ifndef LSH_HASHFUNCTION_H
#define LSH_HASHFUNCTION_H

#include "../../Vector-Modules/Vector/Vector.h"
#include "../../Vector-Modules/Point/Point.h"

/* Class that acts as the h_i() function in the context of the LSH algorithm */

class LSHFunction {

private:
    Vector *unitVector;
    float shift;
    unsigned int window;

public:
    LSHFunction(unsigned int, double, unsigned int);
    ~LSHFunction();
    long long int hash(const Point&) const;

};


#endif

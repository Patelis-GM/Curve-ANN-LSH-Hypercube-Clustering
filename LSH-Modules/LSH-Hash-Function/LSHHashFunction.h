#ifndef LSHHASHFUNCTION_H
#define LSHHASHFUNCTION_H


#include "../LSH-Function/LSHFunction.h"

class LSHHashFunction {

private:
    const static unsigned int PRIME;
    unsigned int k;
    int *weights;
    LSHFunction **lshFunctions;

public:
    LSHHashFunction(unsigned int, double, unsigned int, unsigned int);
    ~LSHHashFunction();
    unsigned int hash(const Point &);
};


#endif

#ifndef HYPERCUBEHASHFUNCTION_H
#define HYPERCUBEHASHFUNCTION_H

#include "../Hypercube-Function/HypercubeFunction.h"

class HypercubeHashFunction {

private:
    unsigned int k;
    HypercubeFunction **hypercubeFunctions;

public:
    HypercubeHashFunction(unsigned int, double, unsigned int, unsigned int);
    ~HypercubeHashFunction();
    std::string hash(const Point &);
};


#endif

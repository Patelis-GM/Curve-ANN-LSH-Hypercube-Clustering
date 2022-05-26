#ifndef RANDOMFUNCTION_H
#define RANDOMFUNCTION_H


#include <random>

/* Class that acts as the f_i() function in the context of the Hypercube algorithm */

class RandomFunction {

private:
    std::default_random_engine *defaultRandomEngine;
    std::uniform_int_distribution<int>* intDistribution;

public:
    RandomFunction();
    ~RandomFunction();
    int coinFlip();

};


#endif

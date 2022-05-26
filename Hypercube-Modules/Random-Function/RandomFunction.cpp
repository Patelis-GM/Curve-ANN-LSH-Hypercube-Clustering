#include <chrono>
#include "RandomFunction.h"

RandomFunction::RandomFunction() {

    this->defaultRandomEngine = new std::default_random_engine (std::chrono::high_resolution_clock::now().time_since_epoch().count());
    this->intDistribution = new std::uniform_int_distribution<>(0, 1);
}


int RandomFunction::coinFlip() {

    /* Return 0 or 1 at random */
    return this->intDistribution->operator()(*this->defaultRandomEngine);
}

RandomFunction::~RandomFunction() {
    delete this->intDistribution;
    delete this->defaultRandomEngine;
}



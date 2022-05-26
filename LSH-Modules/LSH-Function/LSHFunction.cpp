#include <cmath>
#include "LSHFunction.h"
#include "../../Utilities/Random/Random.h"

LSHFunction::LSHFunction(unsigned int dimensions, double norm, unsigned int window) {

    this->unitVector = Vector::randomUnitVector(dimensions, norm);
    this->shift = Random::randomFloat(0.0, float(window));
    this->window = window;
}

LSHFunction::~LSHFunction() {
    delete this->unitVector;
}

long long int LSHFunction::hash(const Point &point) const {

    /* Calculate the <Point,unitVector> and add the LSHFunction's shift to it */
    long double numerator = (*this->unitVector * (Vector &) point) + (long double) this->shift;

    /* Calculate the corresponding bucket/index and return it */
    return (long long int) floorl(numerator / (long double) this->window);
}



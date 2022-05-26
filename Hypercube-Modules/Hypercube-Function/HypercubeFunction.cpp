#include "HypercubeFunction.h"
#include "../../Utilities/Random/Random.h"

HypercubeFunction::HypercubeFunction(unsigned int dimensions, double norm, unsigned int window) {

    this->randomFunction = new RandomFunction();
    this->unitVector = Vector::randomUnitVector(dimensions, norm);
    this->shift = Random::randomFloat(0.0, float(window));
    this->window = window;
}

HypercubeFunction::~HypercubeFunction() {

    delete this->randomFunction;
    delete this->unitVector;
}


std::string HypercubeFunction::hash(const Point &point) {

    /* Calculate the <Point,unitVector> and add the HypercubeFunction's shift to it */
    long double numerator = (*this->unitVector * (Vector &) point) + this->shift;

    /* Calculate the corresponding bucket/index */
    long long int index = floor(numerator / this->window);

    /* If the index is not mapped to 0 or 1 already then map it using the HypercubeFunction's randomFunction
     * and store the value calculated (0/1) for future use */
    if (this->buckets.find(index) == this->buckets.end())
        this->buckets[index] = std::to_string(this->randomFunction->coinFlip());

    /* Return the appropriate value (0/1) */
    return this->buckets[index];
}



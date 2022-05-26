#include <limits>
#include "LSHHashFunction.h"
#include "../../Utilities/Random/Random.h"
#include "../../Utilities/Arithmetic/Arithmetic.h"

const unsigned int LSHHashFunction::PRIME = 4294967291;

LSHHashFunction::LSHHashFunction(unsigned int dimensions, double norm, unsigned int window, unsigned int k) {

    this->k = k;

    /* Create k random weights one for each LSHFunction */
    this->weights = new int[k];
    for (unsigned int i = 0; i < k; ++i)
        this->weights[i] = Random::randomInteger(1, (std::numeric_limits<int>::max() / 2));

    /* Create k LSHFunctions */
    this->lshFunctions = new LSHFunction *[k];
    for (unsigned int i = 0; i < k; ++i)
        this->lshFunctions[i] = new LSHFunction(dimensions, norm, window);

}

LSHHashFunction::~LSHHashFunction() {

    for (unsigned int i = 0; i < this->k; ++i)
        delete this->lshFunctions[i];

    delete[] this->lshFunctions;
    delete[] this->weights;
}

/* The following implementation is based on the fact that : (x+y) mod A == [(x mod A) + (y mod A)] mod A  to prevent potential overflows */
unsigned int LSHHashFunction::hash(const Point &point) {

    /* Calculate the amplified hash value of the given Point */
    long long int amplifiedHashValue = 0;
    for (unsigned int i = 0; i < this->k; ++i)
        amplifiedHashValue += Arithmetic::mod(this->weights[i] * this->lshFunctions[i]->hash(point), LSHHashFunction::PRIME);

    /* Return (amplified hash value mod 4294967291) */
    return (unsigned int) Arithmetic::mod(amplifiedHashValue, LSHHashFunction::PRIME);
}



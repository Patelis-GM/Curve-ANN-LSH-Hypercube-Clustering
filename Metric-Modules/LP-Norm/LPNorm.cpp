#include "LPNorm.h"

LPNorm::LPNorm(long double p) {
    this->p = p;
}

long double LPNorm::distance(const Vector &vectorOne, const Vector &vectorTwo) {
    return VectorMetric::distance(vectorOne, vectorTwo, this->p);
}

LPNorm::~LPNorm() {}



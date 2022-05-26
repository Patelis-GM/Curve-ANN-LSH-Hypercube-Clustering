#include "ContinuousFrechet.h"
#include "include/Frechet.hpp"

ContinuousFrechet::ContinuousFrechet() {}

ContinuousFrechet::~ContinuousFrechet() {}

/* Utility function to calculate the Continuous Frechet distance between 2 Curves according to the library that we were provided with */
long double ContinuousFrechet::distance(const Curve &curveOne, const Curve &curveTwo) const {

    if (curveOne.getComplexity() < 2 || curveTwo.getComplexity() < 2)
        return CurveMetric::INVALID_PARAMETERS;

    return (long double) Frechet::Continuous::distance(curveOne, curveTwo).value;
}




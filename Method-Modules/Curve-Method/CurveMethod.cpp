#include <chrono>
#include "CurveMethod.h"

CurveMethod::CurveMethod(unsigned int dimensions, long double norm, unsigned int k, unsigned int w) : Method(dimensions, norm, k, w) {}

CurveMethod::~CurveMethod() {}

/* Utility function to get the exact neighbor of a Curve given a set of Curves and a Curve metric */
std::tuple<Curve *, long double, double> CurveMethod::getExactNeighbor(Curve *const queryCurve, const std::vector<Curve *> &inputCurves, CurveMetric *curveMetric) {

    long double minDistance = std::numeric_limits<long double>::max();
    Curve *nearestNeighbor = nullptr;

    /* Start the clock */
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    for (Curve *curve: inputCurves) {

        long double distanceToQueryCurve = curveMetric->distance(*queryCurve, *curve);

        if (distanceToQueryCurve < minDistance) {

            minDistance = distanceToQueryCurve;
            nearestNeighbor = curve;
            if (minDistance == 0.0)
                break;
        }
    }

    /* Stop the clock */
    std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();

    auto duration = finish - start;

    return std::make_tuple(nearestNeighbor, minDistance, std::chrono::duration<double, std::milli>(duration).count());
}

/* The following function calculates the optimal delta of a grid */
long double CurveMethod::calculateOptimalDelta(const std::vector<Curve *> &inputCurves, unsigned int gridDimensions, unsigned int exponent) {

    unsigned minComplexity = std::numeric_limits<unsigned int>::max();
    for (Curve *curve: inputCurves)
        if (curve->getComplexity() < minComplexity)
            minComplexity = curve->getComplexity();


    long double delta = 4 * gridDimensions * minComplexity;

    return delta / powl(10, (long double) exponent);

}

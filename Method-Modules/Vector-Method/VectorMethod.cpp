#include <chrono>
#include "VectorMethod.h"

VectorMethod::VectorMethod(unsigned int dimensions, long double norm, unsigned int k, unsigned int w) : Method(dimensions, norm, k, w) {}

VectorMethod::~VectorMethod() {}

/* Utility function to get the exact neighbor of a Point given a set of Points and a Vector metric */
std::tuple<Point *, long double, double> VectorMethod::getExactNeighbor(Point *const queryPoint, const std::vector<Point *> &inputPoints, VectorMetric *vectorMetric) {

    long double minDistance = std::numeric_limits<long double>::max();
    Point *nearestNeighbor = nullptr;

    /* Start the clock */
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    for (Point *point: inputPoints) {

        long double distanceToQueryPoint = vectorMetric->distance(*queryPoint, *point);

        if (distanceToQueryPoint < minDistance) {
            minDistance = distanceToQueryPoint;
            nearestNeighbor = point;
            if (minDistance == 0.0)
                break;
        }
    }

    /* Stop the clock */
    std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();

    auto duration = finish - start;


    return std::make_tuple(nearestNeighbor, minDistance, std::chrono::duration<double, std::milli>(duration).count());
}



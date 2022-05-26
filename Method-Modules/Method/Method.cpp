#include <cmath>
#include "Method.h"

const unsigned int Method::DEFAULT_W = 6;

Method::Method(unsigned int dimensions, long double norm, unsigned int k, unsigned int w) {

    this->dimensions = dimensions;
    this->norm = norm;
    this->k = k;
    this->w = w;
    if (this->w == 0)
        this->w = Method::DEFAULT_W;
}

Method::~Method() {}

/* The following function calculates the optimal window of h_i() */
unsigned int Method::calculateOptimalWindow(double percentage, const std::vector<Point *> &inputPoints, VectorMetric *vectorMetric) {

    /* Arbitrary value to return  */
    if (inputPoints.size() == 1 || inputPoints.empty() || percentage <= 0)
        return Method::DEFAULT_W;

    unsigned int totalPoints = (unsigned int) floor(percentage * (double) inputPoints.size());

    /* In case floor(percentage * points.size()) <= 0 */
    if (totalPoints == 0 || totalPoints > inputPoints.size())
        totalPoints = inputPoints.size() / 2;

    long double averageOfAverages = 0;

    for (unsigned int i = 0; i < totalPoints; ++i) {

        long double pointAverageDistance = 0;
        for (unsigned int j = 0; j < totalPoints; ++j)
            if (j != i)
                pointAverageDistance += vectorMetric->distance(*inputPoints.at(i), *inputPoints.at(j));

        averageOfAverages += (pointAverageDistance / (long double) totalPoints);
    }


    if ((averageOfAverages / (long double) totalPoints) > 0)
        return (unsigned int) (averageOfAverages / (long double) totalPoints);

    else
        return Method::DEFAULT_W;
}

unsigned int Method::getVectorSpaceDimensions() const {
    return this->dimensions;
}





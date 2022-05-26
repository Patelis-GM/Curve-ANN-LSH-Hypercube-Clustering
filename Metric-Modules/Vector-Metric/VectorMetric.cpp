#include "VectorMetric.h"

const long double VectorMetric::INVALID_PARAMETERS = std::numeric_limits<long double>::max();

VectorMetric::~VectorMetric() {}

/* Utility function to calculate the p-distance between 2 Vectors
 * The distance formula is the following : [(x_1 - y_1)^p + (x_2 - y_2)^p + ... + (x_i - y_i)^p]^(1/p) */
long double VectorMetric::distance(const Vector &vectorOne, const Vector &vectorTwo, long double p) {

    if (p < 0 || (vectorOne.getDimensions() != vectorTwo.getDimensions()))
        return VectorMetric::INVALID_PARAMETERS;

    if (vectorOne.equals(vectorTwo))
        return 0.0;

    long double distance;

    /* Chebyshev distance */
    if (p == 0) {

        distance = std::abs(vectorOne.getCoordinates().at(0) - vectorTwo.getCoordinates().at(0));

        for (long unsigned i = 1; i < vectorOne.getDimensions(); ++i) {
            long double newDistance = std::abs(vectorOne.getCoordinates().at(i) - vectorTwo.getCoordinates().at(i));
            if (newDistance > distance)
                distance = newDistance;
        }

    }

    else {

        distance = 0.0;

        for (long unsigned i = 0; i < vectorOne.getDimensions(); ++i)
            distance += powl(vectorOne.getCoordinates().at(i) - vectorTwo.getCoordinates().at(i), p);

        distance = powl(distance, ((long double) 1.0 / p));
    }


    return distance;
}

/* Utility function to calculate the p-norm of a Vector
 * The norm formula is the following : [(x_1)^p + (x_2)^p + ... + (x_i)^p]^(1/p) */
long double VectorMetric::norm(const Vector &vector, long double p) {

    if (p < 0)
        return VectorMetric::INVALID_PARAMETERS;

    std::vector<long double> zeros(vector.getCoordinates().size(), 0.0);
    Vector zeroVector(zeros);

    return VectorMetric::distance(vector, zeroVector, p);

}

/* Utility function to calculate the p-norm of a Vector without calculating the corresponding root
 * The norm_p formula is the following : [(x_1)^p + (x_2)^p + ... + (x_i)^p] */
long double VectorMetric::norm_p(const Vector &vector, long double p) {

    if (p < 0)
        return VectorMetric::INVALID_PARAMETERS;

    std::vector<long double> zeros(vector.getCoordinates().size(), 0.0);
    Vector zeroVector(zeros);

    return VectorMetric::distance_p(vector, zeroVector, p);

}

/* Utility function to calculate the p-distance between 2 Vectors without calculating the corresponding root
 * The distance_p formula is the following : [(x_1 - y_1)^p + (x_2 - y_2)^p + ... + (x_i - y_i)^p] */
long double VectorMetric::distance_p(const Vector &vectorOne, const Vector &vectorTwo, long double p) {


    if (p < 0 || (vectorOne.getDimensions() != vectorTwo.getDimensions()))
        return VectorMetric::INVALID_PARAMETERS;


    if (vectorOne.equals(vectorTwo))
        return 0.0;


    long double distance;

    /* Chebyshev distance */
    if (p == 0) {

        distance = std::abs(vectorOne.getCoordinates().at(0) - vectorTwo.getCoordinates().at(0));

        for (long unsigned i = 1; i < vectorOne.getDimensions(); ++i) {
            long double newDistance = std::abs(vectorOne.getCoordinates().at(i) - vectorTwo.getCoordinates().at(i));
            if (newDistance > distance)
                distance = newDistance;
        }

    }

    else {

        distance = 0.0;
        for (long unsigned i = 0; i < vectorOne.getDimensions(); ++i)
            distance += powl(vectorOne.getCoordinates().at(i) - vectorTwo.getCoordinates().at(i), p);
    }

    return distance;
}




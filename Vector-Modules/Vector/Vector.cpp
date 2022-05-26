#include <random>
#include <chrono>
#include "Vector.h"
#include "../../Metric-Modules/Vector-Metric/VectorMetric.h"

const long double Vector::INVALID_PARAMETERS = std::numeric_limits<long double>::max();

Vector::Vector(const std::vector<long double> &coordinates) {

    for (long double coordinate: coordinates)
        this->coordinates.push_back(coordinate);
}

/* Utility function to create a unit Vector, which is normalised according to the p-norm */
Vector *Vector::randomUnitVector(unsigned int dimensions, double p) {

    if (p < 0)
        return nullptr;

    /* Random number distribution according to a Gaussian distribution */
    std::default_random_engine defaultRandomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::normal_distribution<long double> distribution(0.0, 1.0);

    /* Create |dimensions| random coordinates */
    std::vector<long double> coordinates;
    for (unsigned int i = 0; i < dimensions; ++i)
        coordinates.push_back(distribution(defaultRandomEngine));

    Vector *unitVector = new Vector(coordinates);

    /* Normalise the Vector according to the p-norm */
    long double pNorm = VectorMetric::norm(*unitVector, p);
    for (unsigned int i = 0; i < unitVector->getDimensions(); ++i)
        unitVector->coordinates[i] /= pNorm;

    return unitVector;
}

/* Inner product operator */
long double Vector::operator*(const Vector &vector) const {

    if (vector.getDimensions() != this->getDimensions())
        return Vector::INVALID_PARAMETERS;

    long double innerProduct = 0;
    for (long unsigned i = 0; i < this->coordinates.size(); ++i)
        innerProduct += (this->coordinates.at(i) * vector.coordinates.at(i));

    return innerProduct;
}

unsigned int Vector::getDimensions() const {
    return this->coordinates.size();
}

const std::vector<long double> &Vector::getCoordinates() const {
    return this->coordinates;
}

/* Utility function to check whether a Vector is equal to another Vector */
bool Vector::equals(const Vector &vector) const {

    if (&vector == this)
        return true;

    if (this->coordinates.size() != vector.coordinates.size())
        return false;

    for (unsigned int i = 0; i < this->coordinates.size(); ++i)
        if (this->coordinates[i] != vector.coordinates[i])
            return false;

    return true;
}

/* Utility function to calculate the mean Vector of a set of Vectors */
Vector *Vector::meanVector(const std::vector<Vector *> &vectors) {

    /* The provided set of Vectors should not be empty */
    if (vectors.empty())
        return nullptr;

    /* Each Vector in the set should X-dimensional */
    unsigned int dimensions = vectors[0]->getDimensions();
    for (Vector *vector: vectors)
        if (vector->getDimensions() != dimensions)
            return nullptr;


    std::vector<long double> coordinates;

    for (unsigned int i = 0; i < dimensions; ++i) {

        long double coordinate = 0.0;

        for (unsigned int j = 0; j < vectors.size(); ++j)
            coordinate += vectors[j]->getCoordinates()[i];

        coordinates.push_back(coordinate);
    }

    for (unsigned int i = 0; i < coordinates.size(); ++i)
        coordinates[i] /= vectors.size();

    return new Vector(coordinates);
}

/* Utility - operator defined for the provided Continuous Frechet Distance library */
Vector Vector::operator-(const Vector &vector) const {

    std::vector<long double> vectorCoordinates;

    for (unsigned int i = 0; i < this->getDimensions(); ++i)
        vectorCoordinates.push_back(this->coordinates[i] - vector.coordinates[i]);

    Vector result(vectorCoordinates);

    return result;
}

/* Utility + operator defined for the provided Continuous Frechet Distance library */
Vector Vector::operator+(const Vector &vector) const {

    std::vector<long double> vectorCoordinates;

    for (unsigned int i = 0; i < this->getDimensions(); ++i)
        vectorCoordinates.push_back(this->coordinates[i] + vector.coordinates[i]);

    Vector result(vectorCoordinates);

    return result;
}

/* Utility function defined for the provided Continuous Frechet Distance library */
Interval Vector::ball_intersection_interval(const distance_t distance_sqr, const Vector &line_start, const Vector &line_end) const {

    const Vector u = line_end - line_start, v = *this - line_start;
    const parameter_t ulen_sqr = u.length_sqr(), vlen_sqr = v.length_sqr();

    if (near_eq(ulen_sqr, parameter_t(0))) {
        if (vlen_sqr <= distance_sqr) return Interval(parameter_t(0), parameter_t(1));
        else return Interval();
    }

    const parameter_t p = -2. * ((u * v) / ulen_sqr), q = vlen_sqr / ulen_sqr - distance_sqr / ulen_sqr;

    const parameter_t phalf_sqr = p * p / 4., discriminant = phalf_sqr - q;

    if (discriminant < 0) return Interval();

    const parameter_t discriminant_sqrt = std::sqrt(discriminant);

    const parameter_t minus_p_h = -p / 2., r1 = minus_p_h + discriminant_sqrt, r2 = minus_p_h - discriminant_sqrt;
    const parameter_t lambda1 = std::min(r1, r2), lambda2 = std::max(r1, r2);

    return Interval(std::max(parameter_t(0), lambda1), std::min(parameter_t(1), lambda2));

}

/* Utility function defined for the provided Continuous Frechet Distance library */
distance_t Vector::line_segment_dist_sqr(const Vector &p1, const Vector &p2) const {

    const Vector u = p2 - p1;
    parameter_t projection_param = (*this - p1) * u / (u * u);
    if (projection_param < parameter_t(0)) projection_param = parameter_t(0);
    else if (projection_param > parameter_t(1)) projection_param = parameter_t(1);
    const Vector projection = p1 + u * projection_param;
    return projection.dist_sqr(*this);
}

/* Utility function defined for the provided Continuous Frechet Distance library */
distance_t Vector::dist_sqr(const Vector &vector) const {
    return VectorMetric::distance_p(*this, vector, 2.0);
}

/* Utility function defined for the provided Continuous Frechet Distance library */
distance_t Vector::length_sqr() const {
    return VectorMetric::norm_p(*this, 2.0);
}

/* Utility function to calculate a Vector's std::string representation */
std::string Vector::toString() {

    std::string stringRepresentation = "(";

    for (long unsigned int i = 0; i < this->coordinates.size(); ++i) {
        stringRepresentation += std::to_string(this->coordinates[i]);
        if (i < this->coordinates.size() - 1)
            stringRepresentation += ",";
    }

    stringRepresentation += ")";

    return stringRepresentation;

}

long double Vector::operator[](unsigned int index) const {

    if (index < this->coordinates.size())
        return this->coordinates[index];

    return std::numeric_limits<long double>::max();
}



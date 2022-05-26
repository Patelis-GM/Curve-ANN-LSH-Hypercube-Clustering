#ifndef VECTOR_H
#define VECTOR_H


#include <vector>
#include "../../Metric-Modules/Continuous-Frechet/include/types.hpp"
#include "../../Metric-Modules/Continuous-Frechet/include/interval.hpp"

class Vector {

protected:
    std::vector<long double> coordinates;

public:
    static const long double INVALID_PARAMETERS;
    explicit Vector(const std::vector<long double> &);

    std::string toString();
    bool equals(const Vector &) const;
    unsigned int getDimensions() const;
    const std::vector<long double> &getCoordinates() const;
    long double operator[](unsigned int) const;


    distance_t dist_sqr(const Vector &) const;
    distance_t length_sqr() const;
    distance_t line_segment_dist_sqr(const Vector &, const Vector &) const;
    Interval ball_intersection_interval(const distance_t, const Vector &, const Vector &) const;

    /* Utility template function defined for the provided Continuous Frechet Distance library */
    template<typename T>
    Vector operator*(const T multiplier) const {

        std::vector<long double> vectorCoordinates;

        for (unsigned int i = 0; i < this->getDimensions(); ++i)
            vectorCoordinates.push_back(this->coordinates[i] * multiplier);

        Vector result(vectorCoordinates);

        return result;
    };

    Vector operator-(const Vector &) const;
    Vector operator+(const Vector &) const;
    long double operator*(const Vector &) const;


    static Vector *randomUnitVector(unsigned int, double);
    static Vector *meanVector(const std::vector<Vector *> &);

};


#endif

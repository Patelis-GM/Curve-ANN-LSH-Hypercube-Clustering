#ifndef VECTORMETRIC_H
#define VECTORMETRIC_H


#include "../../Vector-Modules/Vector/Vector.h"

/* Abstract base class to implement any Vector metric */

class VectorMetric {

public:
    const static long double INVALID_PARAMETERS;
    virtual ~VectorMetric();
    /* Pure virtual function as each derived class may calculate distance between two Vectors in a different way */
    virtual long double distance(const Vector &, const Vector &) = 0;
    static long double distance(const Vector &, const Vector &, long double);
    static long double distance_p(const Vector &, const Vector &, long double);
    static long double norm(const Vector &, long double);
    static long double norm_p(const Vector &, long double);
};


#endif

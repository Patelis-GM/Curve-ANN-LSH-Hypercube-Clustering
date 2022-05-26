#ifndef LPNORM_H
#define LPNORM_H


#include "../Vector-Metric/VectorMetric.h"

/* The LPNorm class inherits the VectorMetric class and calculates the distance between 2 Vectors according to the p-norm */

class LPNorm : public VectorMetric {

private:
    long double p = 2.0;

public:
    LPNorm(long double);
    virtual ~LPNorm();
    virtual long double distance(const Vector &, const Vector &);
};


#endif

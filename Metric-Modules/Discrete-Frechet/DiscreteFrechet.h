#ifndef DISCRETEFRECHET_H
#define DISCRETEFRECHET_H


#include "../Curve-Metric/CurveMetric.h"

/* The DiscreteFrechet class inherits the CurveMetric class and calculates the Discrete Frechet distance between 2 Curves */

class DiscreteFrechet : public CurveMetric {

public:
    DiscreteFrechet();
    virtual ~DiscreteFrechet();
    long double distance(const Curve &, const Curve &) const;
    std::vector<std::pair<Vector *, Vector *>>* getOptimalTraversal(const Curve &, const Curve &) const;

};


#endif
#ifndef CONTINUOUSFRECHET_H
#define CONTINUOUSFRECHET_H

#include "../Curve-Metric/CurveMetric.h"

/* The ContinuousFrechet class inherits the CurveMetric class and calculates the Continuous Frechet distance between 2 Curves */

class ContinuousFrechet : public CurveMetric {

public:
    ContinuousFrechet();
    virtual ~ContinuousFrechet();
    long double distance(const Curve &, const Curve &) const;
};


#endif

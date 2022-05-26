#ifndef CURVEMETRIC_H
#define CURVEMETRIC_H

#include "../../Curve-Modules/Curve/Curve.h"

/* Abstract base class to implement any Curve metric */

class CurveMetric {

public:
    const static long double INVALID_PARAMETERS;
    virtual ~CurveMetric();
    /* Pure virtual function as each derived class may calculate distance between two Curves in a different way */
    virtual long double distance(const Curve &, const Curve &) const = 0;
};


#endif

#ifndef CURVEMETHOD_H
#define CURVEMETHOD_H

#include <tuple>
#include "../Method/Method.h"
#include "../../Metric-Modules/Curve-Metric/CurveMetric.h"

/* Abstract base class for CurveLSH  */

class CurveMethod : public Method {


public:
    CurveMethod(unsigned int, long double, unsigned int, unsigned int);
    virtual ~CurveMethod();
    /* Pure virtual function as each derived class may insert a Curve in a different way */
    virtual void insertCurve(Curve *const) = 0;
    virtual std::tuple<Curve *, long double, double> getExactNeighbor(Curve *const, const std::vector<Curve *> &, CurveMetric *);
    /* Pure virtual function as each derived class may perform range search in a different way */
    virtual std::tuple<Curve *, long double, double> getApproximateNearestNeighbor(Curve *const, CurveMetric *) = 0;
    virtual std::vector<Curve *> *rangeSearch(Curve *const, long double, CurveMetric *) = 0;
    static long double calculateOptimalDelta(const std::vector<Curve *> &, unsigned int, unsigned int);

};


#endif
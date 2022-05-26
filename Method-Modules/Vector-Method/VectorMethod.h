#ifndef VECTORMETHOD_H
#define VECTORMETHOD_H


#include "../Method/Method.h"
#include <tuple>

/* Abstract base class for VectorLSH and Hypercube */

class VectorMethod : public Method {

public:
    VectorMethod(unsigned int, long double, unsigned int, unsigned int);
    virtual ~VectorMethod();
    /* Pure virtual function as each derived class may insert a Point in a different way */
    virtual void insertPoint(Point *const) = 0;
    /* Pure virtual function as each derived class may perform range search in a different way */
    virtual std::vector<Point *> *rangeSearch(Point *const, long double, VectorMetric *) = 0;
    virtual std::tuple<Point *, long double, double> getApproximateNearestNeighbor(Point *const, VectorMetric *) = 0;
    virtual std::tuple<Point *, long double, double> getExactNeighbor(Point *const, const std::vector<Point *> &, VectorMetric *);
};


#endif

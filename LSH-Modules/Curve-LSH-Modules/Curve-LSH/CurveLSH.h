#ifndef CURVELSH_H
#define CURVELSH_H


#include "../../../Method-Modules/Curve-Method/CurveMethod.h"
#include "../Curve-LSH-Table/CurveLSHTable.h"

class CurveLSH : public CurveMethod {

private:
    std::vector<CurveLSHTable *> curveLSHTables;
    int threshold;

public:
    CurveLSH(unsigned int, long double, unsigned int, unsigned int, unsigned int, unsigned int, int, unsigned int, long double, bool);
    virtual ~CurveLSH();
    void insertCurve(Curve *const curve);
    virtual std::tuple<Curve *, long double, double> getApproximateNearestNeighbor(Curve *const, CurveMetric *);
    virtual std::vector<Curve *> *rangeSearch(Curve *const, long double, CurveMetric *);
    unsigned int getGridDimensions() const;
};


#endif
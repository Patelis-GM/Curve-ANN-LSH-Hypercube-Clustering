#ifndef VECTORLSH_H
#define VECTORLSH_H


#include "../../../Method-Modules/Vector-Method/VectorMethod.h"
#include "../Vector-LSH-Table/VectorLSHTable.h"

class VectorLSH : public VectorMethod {

private:
    std::vector<VectorLSHTable *> vectorLSHTables;
    int threshold;


public:
    VectorLSH(unsigned int, long double, unsigned int, unsigned int, unsigned int, unsigned int, int);
    virtual ~VectorLSH();

    void insertPoint(Point *const);
    std::tuple<Point *, long double, double> getApproximateNearestNeighbor(Point *const, VectorMetric *);
    std::vector<Point *> *rangeSearch(Point *const, long double, VectorMetric *);
};


#endif

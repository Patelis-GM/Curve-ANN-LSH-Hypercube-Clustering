#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include "../Hypercube-Hash-Function/HypercubeHashFunction.h"
#include "../../Method-Modules/Vector-Method/VectorMethod.h"

class Hypercube : public VectorMethod {

private:
    unsigned int M;
    unsigned int probes;
    HypercubeHashFunction *hypercubeHashFunction;
    std::unordered_map<std::string, std::vector<Point *> *> vertices;

    std::vector<std::string> *getVerticesToVisit(const std::string &);
    std::vector<std::string> *getVerticesWithHammingDistance(const std::string &, unsigned int);

public:
    Hypercube(unsigned int, double, unsigned int, unsigned int, unsigned int, unsigned int);
    ~Hypercube();

    void insertPoint(Point *const);
    std::vector<Point *> *rangeSearch(Point *const, long double, VectorMetric *);
    std::tuple<Point *, long double, double> getApproximateNearestNeighbor(Point *const, VectorMetric *);

};


#endif

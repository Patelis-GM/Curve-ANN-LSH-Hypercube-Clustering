#ifndef CURVECLUSTERINGRESULTS_H
#define CURVECLUSTERINGRESULTS_H


#include "../Curve-Cluster/CurveCluster.h"

class CurveClusteringResults {

private:

    double duration;
    std::vector<CurveCluster *> clusters;
    std::vector<long double> *averageSilhouettes;

public:

    CurveClusteringResults(double, const std::vector<CurveCluster *> &, std::vector<long double> *);
    ~CurveClusteringResults();

    std::vector<long double> *getAverageSilhouettes() const;
    double getDuration() const;
    const std::vector<CurveCluster *> &getClusters() const;

};


#endif
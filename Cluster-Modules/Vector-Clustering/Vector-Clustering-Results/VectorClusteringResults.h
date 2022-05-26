#ifndef VECTORCLUSTERINGRESULTS_H
#define VECTORCLUSTERINGRESULTS_H

#include <vector>
#include "../Vector-Cluster/VectorCluster.h"

class VectorClusteringResults {

private:
    double duration;
    std::vector<VectorCluster *> clusters;
    std::vector<long double> *averageSilhouettes;

public:
    VectorClusteringResults(double, const std::vector<VectorCluster *> &, std::vector<long double> *);
    ~VectorClusteringResults();

    std::vector<long double> *getAverageSilhouettes() const;
    double getDuration() const;
    const std::vector<VectorCluster *> &getClusters() const;

};


#endif

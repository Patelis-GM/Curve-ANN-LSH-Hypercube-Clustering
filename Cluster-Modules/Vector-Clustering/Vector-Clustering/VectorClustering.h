#ifndef VECTORCLUSTERING_H
#define VECTORCLUSTERING_H

#include <vector>
#include <unordered_map>
#include "../../Clustering-Parameters/ClusteringParameters.h"
#include "../../../Vector-Modules/Point/Point.h"
#include "../Vector-Cluster/VectorCluster.h"
#include "../Vector-Clustering-Results/VectorClusteringResults.h"
#include "../../../Method-Modules/Vector-Method/VectorMethod.h"


class VectorClustering {

private:
    unsigned int totalIterations;
    std::vector<Point *> *inputPoints;
    std::vector<VectorCluster *> clusters;

    void initializeCentroids(unsigned int, VectorMetric *);
    long double calculateInitialRadius(VectorMetric *);

    unsigned int binarySearch(const std::vector<long double> &, long double);
    void Lloyd(std::unordered_map<Point *, bool> *, unsigned int, VectorMetric *);
    void reverseRangeSearch(VectorMethod *, VectorMetric *, long double);
    std::vector<long double> *averageSilhouettes(VectorMetric *);

public:
    VectorClustering();
    ~VectorClustering();

    VectorClusteringResults *apply(unsigned int, VectorMethod *, VectorMetric *, unsigned int, std::vector<Point *> *,bool);
};


#endif
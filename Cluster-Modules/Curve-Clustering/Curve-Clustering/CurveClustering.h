#ifndef CURVE_CLUSTERING
#define CURVE_CLUSTERING

#include <string>
#include <unordered_map>
#include "../../../Curve-Modules/Curve/Curve.h"
#include "../Curve-Cluster/CurveCluster.h"
#include "../../../Metric-Modules/Curve-Metric/CurveMetric.h"
#include "../Curve-Clustering-Results/CurveClusteringResults.h"
#include "../../Clustering-Parameters/ClusteringParameters.h"
#include "../../../Method-Modules/Curve-Method/CurveMethod.h"


class CurveClustering {

private:
    unsigned int totalIterations;
    std::vector<Curve *> *inputCurves;
    std::vector<CurveCluster *> clusters;
    unsigned int vectorSpaceDimensions;

    unsigned int binarySearch(const std::vector<long double> &, long double);
    std::vector<long double> *averageSilhouettes(CurveMetric *);
    void reverseRangeSearch(long double, CurveMetric *, CurveMethod *);
    long double calculateInitialRadius(CurveMetric *);
    void Lloyd(std::unordered_map<Curve *, bool> *, unsigned int, CurveMetric *);
    void initializeCentroids(unsigned int totalClusters, CurveMetric *);

public:
    CurveClustering();
    CurveClusteringResults *apply(unsigned int, CurveMethod *, CurveMetric *, std::vector<Curve *> *, unsigned int, unsigned int, bool);
    ~CurveClustering();

};


#endif
#ifndef APPLIEDCLUSTERING_H
#define APPLIEDCLUSTERING_H

#include <string>
#include "../../Cluster-Modules/Vector-Clustering/Vector-Clustering-Results/VectorClusteringResults.h"
#include "../../Cluster-Modules/Curve-Clustering/Curve-Clustering-Results/CurveClusteringResults.h"


class AppliedClustering{

    private:

    static const long double norm;
    static std::string requestOutputFilePath();
    static void getVectorialRepresentations(const std::vector<Curve *>&, std::vector<Point *>&);
    static void printVectorClusteringResults(const std::string&, const std::string&, VectorClusteringResults*, bool, bool);
    static void printCurveClusteringResults(const std::string&, const std::string&, CurveClusteringResults*, bool, bool);

    public:

    static void appliedClustering(int, char **);

};


#endif

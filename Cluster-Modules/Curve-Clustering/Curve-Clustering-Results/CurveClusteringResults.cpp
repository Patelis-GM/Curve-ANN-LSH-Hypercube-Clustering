#include "CurveClusteringResults.h"

CurveClusteringResults::CurveClusteringResults(double duration, const std::vector<CurveCluster *> &clusters, std::vector<long double> *averageSilhouettes) {
    this->duration = duration;
    this->clusters = clusters;
    this->averageSilhouettes = averageSilhouettes;
}

double CurveClusteringResults::getDuration() const {
    return this->duration;
}

const std::vector<CurveCluster *> &CurveClusteringResults::getClusters() const {
    return this->clusters;
}

std::vector<long double> *CurveClusteringResults::getAverageSilhouettes() const {
    return this->averageSilhouettes;
}

CurveClusteringResults::~CurveClusteringResults() {

    if (this->averageSilhouettes != nullptr)
        delete this->averageSilhouettes;

}

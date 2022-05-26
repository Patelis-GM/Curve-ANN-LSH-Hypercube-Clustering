#include "VectorClusteringResults.h"

VectorClusteringResults::VectorClusteringResults(double duration, const std::vector<VectorCluster *> &clusters, std::vector<long double> *averageSilhouettes) {
    this->duration = duration;
    this->clusters = clusters;
    this->averageSilhouettes = averageSilhouettes;
}

double VectorClusteringResults::getDuration() const {
    return this->duration;
}

const std::vector<VectorCluster *> &VectorClusteringResults::getClusters() const {
    return this->clusters;
}

std::vector<long double> *VectorClusteringResults::getAverageSilhouettes() const {
    return this->averageSilhouettes;
}

VectorClusteringResults::~VectorClusteringResults() {
    if (this->averageSilhouettes != nullptr)
        delete this->averageSilhouettes;
}
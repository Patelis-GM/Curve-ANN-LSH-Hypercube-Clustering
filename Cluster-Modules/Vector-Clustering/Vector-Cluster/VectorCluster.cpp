#include "VectorCluster.h"


VectorCluster::VectorCluster(unsigned int dimensions, const Point &point) {

    this->dimensions = dimensions;
    this->centroid = new Point(point.getCoordinates());
}

VectorCluster::~VectorCluster() {

    delete this->centroid;
}

Point *const VectorCluster::getCentroid() const {
    return this->centroid;
}

const std::vector<Point *> *VectorCluster::getAssignedPoints() const {
    return &this->assignedPoints;
}

void VectorCluster::updateCentroid(const std::vector<Point *> &clusterPoints) {

    if (!clusterPoints.empty()) {

        /* Array that stores the coordinates of the new Centroid */
        std::vector<long double> newCoordinates(this->dimensions, 0.0);

        /* Add up the coordinates of the Points provided */
        for (Point *point: clusterPoints)
            for (unsigned int i = 0; i < this->dimensions; ++i)
                newCoordinates[i] += point->getCoordinates()[i];

        /* Add up the coordinates of the VectorCluster's assigned Points */
        for (Point *point: this->assignedPoints)
            for (unsigned int i = 0; i < this->dimensions; ++i)
                newCoordinates[i] += point->getCoordinates()[i];


        /* Calculate the average value of the new Centroid */
        for (unsigned int i = 0; i < this->dimensions; ++i)
            newCoordinates[i] /= (this->assignedPoints.size() + clusterPoints.size());

        /* Delete the previously dynamically allocated Centroid and create a new one */
        delete this->centroid;
        this->centroid = new Point(newCoordinates);
    }
}

void VectorCluster::assignPoint(Point *assignedPoint) {
    this->assignedPoints.push_back(assignedPoint);
}




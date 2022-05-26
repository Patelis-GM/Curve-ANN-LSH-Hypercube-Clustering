#ifndef VECTORCLUSTER_H
#define VECTORCLUSTER_H

#include <vector>
#include "../../../Vector-Modules/Point/Point.h"

class VectorCluster {

private:
    unsigned int dimensions;
    Point *centroid;
    std::vector<Point *> assignedPoints;

public:
    VectorCluster(unsigned int, const Point &);
    ~VectorCluster();

    Point *const getCentroid() const;
    const std::vector<Point *> *getAssignedPoints() const;
    void updateCentroid(const std::vector<Point *> &);
    void assignPoint(Point *);
};


#endif
#ifndef CURVE_CLUSTER
#define CURVE_CLUSTER

#include "../../../Curve-Modules/Curve/Curve.h"


class CurveCluster {

private:

    Curve *centroid;
    bool shouldDeleteCentroid;
    std::vector<Curve *> assignedCurves;


public:
    CurveCluster(Curve *);
    ~CurveCluster();

    Curve *getCentroid();
    const std::vector<Curve *> *getAssignedCurves() const;
    void updateCentroid(std::vector<Curve *> &, unsigned int);
    void assignCurve(Curve *);

};


#endif

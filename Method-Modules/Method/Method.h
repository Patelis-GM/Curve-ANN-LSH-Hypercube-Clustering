#ifndef METHOD_H
#define METHOD_H

#include <vector>
#include "../../Vector-Modules/Point/Point.h"
#include "../../Metric-Modules/Vector-Metric/VectorMetric.h"

/* Base class for VectorMethod and CurveMethod */

class Method {

private:
    static const unsigned int DEFAULT_W;

protected:
    unsigned int dimensions;
    long double norm;
    unsigned int k;
    unsigned int w;

public:
    Method(unsigned int, long double, unsigned int, unsigned int);
    virtual ~Method();
    static unsigned int calculateOptimalWindow(double, const std::vector<Point *> &, VectorMetric *);
    unsigned int getVectorSpaceDimensions() const;
};


#endif

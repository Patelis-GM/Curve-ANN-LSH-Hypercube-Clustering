#ifndef POINT_H
#define POINT_H

#include <string>
#include "../Vector/Vector.h"

/* Forward declaration of Curve class */
class Curve;


class Point : public Vector {

private:
    Curve *curve;

public:
    explicit Point(const std::vector<long double> &);
    Point(Curve *, const std::vector<long double> &);
    Curve *getCurve() const;

};


#endif

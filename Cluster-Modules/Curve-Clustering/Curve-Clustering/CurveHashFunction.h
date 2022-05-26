#ifndef CURVE_HASH_FUNCTION
#define CURVE_HASH_FUNCTION

#include "../../../Curve-Modules/Curve/Curve.h"


class CurveHashFunction {
public:
    unsigned long int operator()(const std::pair<Curve *, Curve *> &) const;
};


#endif

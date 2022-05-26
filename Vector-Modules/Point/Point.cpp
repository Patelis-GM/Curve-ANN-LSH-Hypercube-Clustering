#include "Point.h"

Point::Point(const std::vector<long double> &coordinates) : Vector(coordinates) {
    this->curve = nullptr;
}


Point::Point(Curve *curve, const std::vector<long double> &coordinates) : Vector(coordinates) {
    this->curve = curve;
}

Curve *Point::getCurve() const {
    return this->curve;
}






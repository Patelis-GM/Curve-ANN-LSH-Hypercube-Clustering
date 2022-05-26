#include "CurveHashFunction.h"

/* Utility operator to hash std::pair<Curve*, Curve*>
 * See Clustering::averageSilhouettes for more */

unsigned long int CurveHashFunction::operator()(const std::pair<Curve *, Curve *> &pair) const {
    unsigned long int hashValueOne = std::hash<Curve *>{}(pair.first);
    unsigned long int hashValueTwo = std::hash<Curve *>{}(pair.second);
    return hashValueOne ^ hashValueTwo;
}
#include "VectorHashFunction.h"

/* Utility operator to hash std::pair<Point*, Point*>
 * See Clustering::averageSilhouettes for more */

unsigned long int VectorHashFunction::operator()(const std::pair<Point*, Point*>& pair) const
{
    unsigned long int hashValueOne = std::hash<Point*>{}(pair.first);
    unsigned long int hashValueTwo = std::hash<Point*>{}(pair.second);
    return hashValueOne ^ hashValueTwo;
}


#ifndef VECTOR_HASH_FUNCTION
#define VECTOR_HASH_FUNCTION

#include "../../../Vector-Modules/Point/Point.h"

class VectorHashFunction {
public:
    unsigned long int operator()(const std::pair<Point*, Point*>& pair) const;
};


#endif


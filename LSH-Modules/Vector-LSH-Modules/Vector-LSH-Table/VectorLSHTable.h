#ifndef VECTORLSHTABLE_H
#define VECTORLSHTABLE_H

#include <vector>
#include "../../../Vector-Modules/Point/Point.h"
#include "../../LSH-Hash-Function/LSHHashFunction.h"

/* Class that acts as the ith hash table - where 0 < i <= L - in the context of the VectorLSH algorithm */

class VectorLSHTable {

private:
    unsigned int id;
    unsigned int size;
    LSHHashFunction *lshHashFunction;
    std::vector<std::pair<unsigned int, Point *>> *buckets;
    unsigned int hashPoint(const Point &);

public:
    VectorLSHTable(unsigned int, unsigned int, double, unsigned int, unsigned int, unsigned int);
    ~VectorLSHTable();
    void insert(Point *const);
    std::vector<Point *> *getSimilarPoints(Point *const, int);
};


#endif

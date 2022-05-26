#ifndef CURVELSHTABLE_H
#define CURVELSHTABLE_H


#include "../../LSH-Hash-Function/LSHHashFunction.h"
#include "../Grid/Grid.h"

/* Class that acts as the ith hash table - where 0 < i <= L - in the context of the CurveLSH algorithm */

class CurveLSHTable {

private:
    unsigned int id;
    unsigned int size;
    LSHHashFunction *lshHashFunction;
    Grid *grid;
    /* Data structure to store the vectorial representation V of a Curve's Grid Curve alongside V's amplified hash value */
    std::vector<std::pair<unsigned int, Point *>> *buckets;
    std::pair<unsigned int, Point *> hashCurve(Curve *) const;

public:
    CurveLSHTable(unsigned int, unsigned int, double, unsigned int, unsigned int, unsigned int, unsigned int, long double, bool);
    virtual ~CurveLSHTable();
    void insert(Curve *const);
    std::vector<Curve *> *getSimilarCurves(Curve *const, int);
    unsigned int getGridDimensions() const;
};


#endif

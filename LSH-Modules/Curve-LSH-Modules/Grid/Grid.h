#ifndef GRID_H
#define GRID_H


#include "../../../Curve-Modules/Curve/Curve.h"

class Grid {

private:
    static const long double LARGE_PADDING_NUMBER;
    bool continuous;
    Vector *t;
    long double delta;
    unsigned int dimensions;
    unsigned int vectorSpaceDimensions;
    void createT();
    Grid(unsigned int, unsigned int, long double, bool);

public:
    static Grid *createGrid(unsigned int, unsigned int, long double, bool);
    ~Grid();
    Point *snap(Curve *curve) const;
    unsigned int getDimensions() const;
};


#endif

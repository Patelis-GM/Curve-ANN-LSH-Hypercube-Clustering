#ifndef CURVE_H
#define CURVE_H

#include <vector>
#include <string>
#include "../../Vector-Modules/Point/Point.h"

class Curve{

private:
    static const long double LARGE_PADDING_NUMBER;

    std::string id;
    unsigned int dimensions;
    std::vector<Vector *> points;
    Point *vectorialRepresentation;
    bool shouldCalculateVectorialRepresentation;
    void calculateVectorialRepresentation();

    /* Private constructors to prevent the creation of an X-dimensional Curve where X > 2 */
    Curve(unsigned int);
    Curve(const std::string &, unsigned int);

public:
    static Curve *createCurve(unsigned int);
    static Curve *createCurve(const std::string &, unsigned int);
    ~Curve();

    void insert(Vector *);
    void filter(long double);

    Vector &operator[](unsigned int) const;

    Point *getVectorialRepresentation();
    unsigned int getComplexity() const;
    unsigned int getDimensions() const;
    const std::string &getID() const;
    const std::vector<Vector *> &getPoints() const;
    std::string toString() const;

};


#endif
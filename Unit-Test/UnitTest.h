#ifndef UNITTEST_H
#define UNITTEST_H

#include "../Curve-Modules/Curve/Curve.h"

class UnitTest {

private:
    static Curve* createRandomCurve(unsigned int,unsigned int);


public:
    static void testVector();
    static void testDiscreteSnapping();
    static void testGrid();
    static void testFrechet();
    static void testArithmetic(void);
    static void testCurve(void);
    static void testRandom(void);
    static void testString(void);
};


#endif

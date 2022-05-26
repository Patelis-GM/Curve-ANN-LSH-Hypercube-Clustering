#include "UnitTest.h"
#include "../Metric-Modules/Vector-Metric/VectorMetric.h"
#include "../Utilities/Random/Random.h"
#include "../LSH-Modules/Curve-LSH-Modules/Grid/Grid.h"
#include "../Metric-Modules/Discrete-Frechet/DiscreteFrechet.h"
#include "../Metric-Modules/Continuous-Frechet/ContinuousFrechet.h"
#include "../Utilities/String/String.h"
#include "../Utilities/Arithmetic/Arithmetic.h"
#include <CUnit/CUnit.h>
#include <random>

/* Utility function to test the Vector module */
void UnitTest::testVector(void) {

    Vector vector1({1, 2, 3});
    Vector vector2({1, 2, 3});
    Vector vector3({1, 2});


    /* A Vector should be equal to itself */
    CU_ASSERT(vector1.equals(vector1));

    /* Vectors of different dimensions can't be equal */
    CU_ASSERT(!vector3.equals(vector1));

    /* The inner product of Vectors of different dimensions can't be defined */
    CU_ASSERT(vector1 * vector3 == Vector::INVALID_PARAMETERS);

    /* <V,V> = |V|^2 */
    CU_ASSERT(vector1 * vector2 == VectorMetric::norm_p(vector1, 2.0));

    /* Testing of the +/-/* operators */
    Vector vector4 = vector1 + vector2;
    Vector vector5 = vector2 - vector1;
    Vector vector6 = vector1 * 2.0;

    Vector result1({2, 4, 6});
    Vector result2({0, 0, 0});

    CU_ASSERT(vector4.equals(result1));
    CU_ASSERT(vector5.equals(result2));
    CU_ASSERT(vector6.equals(result1));

    /* Testing of the mean Vector */
    Vector *meanVector = Vector::meanVector({&vector1, &vector2});
    CU_ASSERT(meanVector->equals(vector1));
    delete meanVector;


    /* The p-norm of a random unit Vector -normalised according to the p-norm - should be equal to 1 */
    long double norm = 2.0;
    Vector *unitVector = Vector::randomUnitVector(10, norm);
    CU_ASSERT_DOUBLE_EQUAL(VectorMetric::norm_p(*unitVector, norm), 1.0, 0.0001);
    delete unitVector;
}


/* Utility function to test whether the Discrete-Snapping mapping is the closest one to the corresponding coordinate */
void UnitTest::testDiscreteSnapping() {

    unsigned int totalTests = 5;
    long double delta = Random::randomLongDouble(1.0, 50.0);
    long double shift = Random::randomLongDouble(0, delta);


    for (unsigned int i = 0; i < totalTests; ++i) {

        long double coordinate = Random::randomLongDouble(-50.0, 50.0);
        long double ai = std::round((coordinate - shift) / delta);
        long double mapping = (ai * delta) + shift;

        /* Given the mapping M then either M, (M+delta) or (M-delta) will be the closest one to the corresponding coordinate  */
        long double distance_1 = std::abs(mapping - coordinate);
        long double distance_2 = std::abs((mapping + delta) - coordinate);
        long double distance_3 = std::abs((mapping - delta) - coordinate);
        long double minDistance = std::min(std::min(distance_1, distance_2), distance_3);

        /* M should be the mapping closest to the corresponding coordinate  */
        CU_ASSERT(distance_1 == minDistance);
    }

}

/* Utility function to test the Grid module */
void UnitTest::testGrid() {


    bool continuous = false;
    unsigned int vectorSpaceDimensions = 11;
    unsigned int gridDimensions = 2;
    long double delta = 0.05;
    Grid *grid = Grid::createGrid(vectorSpaceDimensions, gridDimensions, delta, continuous);

    Curve *curve = Curve::createCurve(gridDimensions);
    std::vector<Vector *> curvePoints = {new Vector({1, 2}), new Vector({2, 3}), new Vector({3, 4})};
    for (Vector *vector: curvePoints)
        curve->insert(vector);

    /* The vector representation of the Grid Curve should be padded to 12 dimensions */
    Point *gridCurve = grid->snap(curve);
    CU_ASSERT(gridCurve->getDimensions() == vectorSpaceDimensions);
    delete gridCurve;
    delete grid;


    /* The vector representation of the Grid Curve should be trimmed to 1 dimension */
    vectorSpaceDimensions = 1;
    grid = Grid::createGrid(vectorSpaceDimensions, gridDimensions, delta, continuous);
    gridCurve = grid->snap(curve);
    CU_ASSERT(gridCurve->getDimensions() == vectorSpaceDimensions);
    delete gridCurve;
    delete grid;
    delete curve;


    /* The vector representation of the Grid Curve should be padded to 12 dimensions */
    curvePoints.clear();
    vectorSpaceDimensions = 12;
    gridDimensions = 1;
    continuous = true;
    curve = Curve::createCurve(gridDimensions);
    curvePoints = {new Vector({1}), new Vector({2}), new Vector({3})};
    for (Vector *vector: curvePoints)
        curve->insert(vector);
    grid = Grid::createGrid(vectorSpaceDimensions, gridDimensions, delta, continuous);
    gridCurve = grid->snap(curve);
    CU_ASSERT(gridCurve->getDimensions() == vectorSpaceDimensions);
    delete gridCurve;
    delete grid;


    /* The vector representation of the Grid Curve should be trimmed to 1 dimension */
    vectorSpaceDimensions = 1;
    grid = Grid::createGrid(vectorSpaceDimensions, gridDimensions, delta, continuous);
    gridCurve = grid->snap(curve);
    CU_ASSERT(gridCurve->getDimensions() == vectorSpaceDimensions);
    delete gridCurve;
    delete grid;


    delete curve;

}


/* Utility function to test whether the following stands : Discrete/Continuous-Frechet-Distance(x,x) = 0 */
void UnitTest::testFrechet() {

    DiscreteFrechet discreteFrechet;
    ContinuousFrechet continuousFrechet;

    unsigned int curveDimensions = 2;
    unsigned int complexity = 20;
    Curve *curve = UnitTest::createRandomCurve(curveDimensions, complexity);

    /* Each one of the following Frechet distances should be equal to 0 */
    CU_ASSERT_DOUBLE_EQUAL(discreteFrechet.distance(*curve, *curve), 0.0, 0.000001);
    CU_ASSERT_DOUBLE_EQUAL(continuousFrechet.distance(*curve, *curve), 0.0, 0.000001);

    delete curve;

    curveDimensions = 1;

    /* Each one of the following Frechet distances should be equal to 0 */
    curve = UnitTest::createRandomCurve(curveDimensions, curveDimensions);
    CU_ASSERT_DOUBLE_EQUAL(discreteFrechet.distance(*curve, *curve), 0.0, 0.000001);
    CU_ASSERT_DOUBLE_EQUAL(continuousFrechet.distance(*curve, *curve), 0.0, 0.000001);


    delete curve;
}

/* Utility function to test whether the String module */
void UnitTest::testString(void) {

    /* Test String::tokenizeString() */
    std::string stringOne = "Hello World";
    std::string stringTwo = "Hello World !";

    std::vector<std::string> *tokens = String::tokenizeString(stringOne, ' ');
    CU_ASSERT(tokens->size() == 2);
    delete tokens;

    tokens = String::tokenizeString(stringTwo, ' ');
    CU_ASSERT(tokens->size() != 2);
    delete tokens;

    /* Test  String::isDigit() */
    char symbol1 = '5';
    char symbol2 = '!';
    CU_ASSERT(String::isDigit(symbol1));
    CU_ASSERT(!String::isDigit(symbol2));

    /* Test String::isBoolean() */
    std::string boolean1 = "false";
    std::string boolean2 = "";
    CU_ASSERT(String::isBoolean(boolean1));
    CU_ASSERT(!String::isBoolean(boolean2));


    /* Test String::hammingDistance() */
    std::string point1 = "101";
    std::string point2 = "010";
    std::string point3 = "0";
    CU_ASSERT(String::hammingDistance(point1, point2) == 3);
    CU_ASSERT(String::hammingDistance(point1, point3) == String::INVALID_PARAMETERS);


    /* Test String::isFloatingPointNumber() */
    std::string floatingNumber1 = "1.234";
    std::string floatingNumber2 = "abc";
    CU_ASSERT(String::isFloatingPointNumber(floatingNumber1));
    CU_ASSERT(!String::isFloatingPointNumber(floatingNumber2));


    /* Test String::isWholeNumber() */
    std::string wholeNumber1 = "5";
    std::string wholeNumber2 = "2.3";
    CU_ASSERT(String::isWholeNumber(wholeNumber1));
    CU_ASSERT(!String::isWholeNumber(wholeNumber2));

}

/* Utility function to test the Arithmetic module */
void UnitTest::testArithmetic(void) {

    /* Test mod's property : (A+B) MOD M = ((A MOD M) + (B MOD M)) MOD M */
    unsigned A = Random::randomInteger(1, std::numeric_limits<int>::max());
    unsigned B = Random::randomInteger(1, std::numeric_limits<int>::max());
    unsigned int M = 4294967291;

    CU_ASSERT(Arithmetic::mod(A + B, M) == Arithmetic::mod(Arithmetic::mod(A, M) + Arithmetic::mod(B, M), M));
    CU_ASSERT(Arithmetic::mod(Arithmetic::mod(A, M) + Arithmetic::mod(B, M), M) < M);

}

/* Utility function to test the Random module */
void UnitTest::testRandom(void) {

    int randomInteger = Random::randomInteger(1, 2);
    CU_ASSERT(randomInteger >= 1);
    CU_ASSERT(randomInteger <= 2);

    float randomFloat = Random::randomFloat(1.001, 1.002);
    CU_ASSERT(randomFloat >= 1.001);
    CU_ASSERT(randomFloat < 1.002);

    double randomDouble = Random::randomDouble(2.001, 2.002);
    CU_ASSERT(randomDouble >= 2.001);
    CU_ASSERT(randomDouble < 2.002);

    long double randomLongDouble = Random::randomLongDouble(3.001, 3.002);
    CU_ASSERT(randomLongDouble >= 3.001);
    CU_ASSERT(randomLongDouble < 3.002);
}

/* Utility function to test the Curve module */
void UnitTest::testCurve(void) {

    /* Create a 1-dimensional Curve and filter it */
    Curve *curveOne = Curve::createCurve(1);

    Vector *v1 = new Vector({1});
    Vector *v2 = new Vector({2});
    Vector *v3 = new Vector({3});
    Vector *v4 = new Vector({4});
    Vector *v5 = new Vector({5});

    curveOne->insert(v1);
    curveOne->insert(v2);
    curveOne->insert(v3);
    curveOne->insert(v4);
    curveOne->insert(v5);

    curveOne->filter(1);
    CU_ASSERT(curveOne->getComplexity() == 3);

    std::vector<Vector *> curvesPoints = curveOne->getPoints();
    CU_ASSERT(curvesPoints[0]->equals(*v1));
    CU_ASSERT(curvesPoints[1]->equals(*v3));
    CU_ASSERT(curvesPoints[2]->equals(*v5));

    /* Create a 2-dimensional Curve and filter it */
    Curve *curveTwo = Curve::createCurve(2);

    Vector *v6 = new Vector({0, 1});
    Vector *v7 = new Vector({1, 2});
    Vector *v8 = new Vector({2, 3});
    Vector *v9 = new Vector({3, 4});
    Vector *v10 = new Vector({4, 5});

    curveTwo->insert(v6);
    curveTwo->insert(v7);
    curveTwo->insert(v8);
    curveTwo->insert(v9);
    curveTwo->insert(v10);

    curveTwo->filter(2);
    CU_ASSERT(curveTwo->getComplexity() == 3);

    curvesPoints = curveTwo->getPoints();
    CU_ASSERT(curvesPoints[0]->equals(*v6));
    CU_ASSERT(curvesPoints[1]->equals(*v8));
    CU_ASSERT(curvesPoints[2]->equals(*v10));

    delete curveOne;
    delete curveTwo;

}

/* Utility function to create a random Curve */
Curve *UnitTest::createRandomCurve(unsigned int dimensions, unsigned int complexity) {

    if (complexity <= 1)
        complexity = 2;


    if (dimensions == 0 || dimensions > 2)
        dimensions = 1;


    Curve *curve = Curve::createCurve(dimensions);
    long double xCoordinate = 0.0;

    for (unsigned int i = 0; i < complexity; ++i) {

        std::vector<long double> coordinates;

        if (dimensions == 2) {
            coordinates.push_back(xCoordinate);
            xCoordinate += 1.0;
        }

        coordinates.push_back(Random::randomLongDouble(1, 100.0));
        curve->insert(new Vector(coordinates));
    }


    return curve;
}

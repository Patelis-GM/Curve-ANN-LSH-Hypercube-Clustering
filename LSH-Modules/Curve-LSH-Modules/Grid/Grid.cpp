#include <cmath>
#include "Grid.h"
#include "../../../Utilities/Random/Random.h"

const long double Grid::LARGE_PADDING_NUMBER = 10000.0;

/* ----
 *
 * README :
 * Each Grid exists in the context of a CurveLSHTable which has an amplified-hash-function that consists of k hash-functions hi().
 * Each hi() has a unit vector say U of dimensions say D and when the Grid calculates the vectorial representation say V of a Grid Curve
 * V's dimensions should be D so that the inner product <U,V> can be defined to hash V
 *
 * ---- */


/* Utility/Guard function to make sure that only a 1/2-dimensional continuous Grid can be created and an X-dimensional discrete Grid can be created where X > 0 */
Grid *Grid::createGrid(unsigned int vectorSpaceDimensions, unsigned int dimensions, long double delta, bool continuous) {

    if (dimensions == 0 || delta <= 0 || vectorSpaceDimensions == 0)
        return nullptr;

    if (continuous && dimensions > 2)
        return nullptr;

    return new Grid(vectorSpaceDimensions, dimensions, delta, continuous);
}


Grid::Grid(unsigned int vectorSpaceDimensions, unsigned int dimensions, long double delta, bool continuous) {


    this->delta = delta;
    this->continuous = continuous;
    this->dimensions = dimensions;
    this->vectorSpaceDimensions = vectorSpaceDimensions;
    this->createT();
}

Grid::~Grid() { delete this->t; }

void Grid::createT() {

    std::vector<long double> coordinates;

    for (unsigned int i = 0; i < this->dimensions; ++i)
        coordinates.push_back(Random::randomLongDouble(0.0, this->delta));

    this->t = new Vector(coordinates);
}

/* Utility function that snap a Curve to a Grid Curve say X and returns X's vectorial representation */
Point *Grid::snap(Curve *curve) const {

    /* Discrete case */
    if (!this->continuous) {

        if (this->dimensions != curve->getDimensions())
            return nullptr;

        std::vector<Vector *> gridCurve;

        /* Map each X-dimensional Vector/Point of the given Curve to the corresponding mapping */
        for (Vector *vector: curve->getPoints()) {

            std::vector<long double> coordinates;

            for (unsigned int i = 0; i < vector->getDimensions(); ++i) {
                long double ai = std::round((vector->getCoordinates()[i] - this->t->getCoordinates()[i]) / this->delta);
                coordinates.push_back((ai * this->delta) + this->t->getCoordinates()[i]);
            }

            gridCurve.push_back(new Vector(coordinates));
        }

        /* Remove any consecutive duplicates */
        for (unsigned int i = 1; i < gridCurve.size(); ++i)
            if (gridCurve[i]->equals(*gridCurve[i - 1])) {
                delete gridCurve[i];
                gridCurve.erase(gridCurve.begin() + i);
                i--;
            }

        /* Create the vectorial representation of the Grid Curve */
        std::vector<long double> coordinates;
        for (Vector *vector: gridCurve) {
            for (long double coordinate: vector->getCoordinates())
                coordinates.push_back(coordinate);
            delete vector;
        }

        /* Padding of the Grid Curve */
        while (coordinates.size() < this->vectorSpaceDimensions)
            coordinates.push_back(Grid::LARGE_PADDING_NUMBER);

        /* Trim of the Grid Curve */
        while (coordinates.size() > this->vectorSpaceDimensions)
            coordinates.erase(coordinates.begin() + (coordinates.size() - 1));

        return new Point(curve, coordinates);
    }

        /* Continuous case */
    else {

        if (this->dimensions != curve->getDimensions() || this->dimensions > 2)
            return nullptr;

        std::vector<Vector *> gridCurve;

        /* Map each 1/2-dimensional Vector/Point of the given Curve to the corresponding mapping */
        for (Vector *vector: curve->getPoints()) {

            std::vector<long double> coordinates;

            for (unsigned int i = 0; i < vector->getDimensions(); ++i) {
                long double mapping = std::floor((vector->getCoordinates()[i] + this->t->getCoordinates()[i]) / this->delta) * this->delta;
                coordinates.push_back(mapping);
            }

            gridCurve.push_back(new Vector(coordinates));
        }

        /* Keep a sequence of minima-maxima and given a set of Points e.g {A,B,C,D,E} then one of the following scenarios will occur :
         * 1) B does not get removed and the next triplet to examine is {B,C,D}
         * 2) B does get removed and the next triplet to examine is {C,D,E} */
        for (unsigned int i = 1; i < gridCurve.size(); ++i) {

            if (i + 1 >= gridCurve.size())
                break;

            long double previous = gridCurve[i - 1]->getCoordinates()[this->dimensions - 1];
            long double current = gridCurve[i]->getCoordinates()[this->dimensions - 1];
            long double next = gridCurve[i + 1]->getCoordinates()[this->dimensions - 1];

            if (current <= std::max(previous, next) && current >= std::min(previous, next)) {
                delete gridCurve[i];
                gridCurve.erase(gridCurve.begin() + i);
            }

        }

        /* Remove any duplicate minima-maxima as shown in the cases below :
         * a) Grid Curve = [1,1,10,10,...] -> Grid Curve = [1,10,...]
         * b) Grid Curve = [(0,1),(0,1),(1,1),(0,10),(0,10)...] -> Grid Curve = [(0,1),(1,1),(0,10)...]
         * In case (b) (1,1) is not removed although its Y value is equal to (0,1) to be able to store more information  */
        for (unsigned int i = 1; i < gridCurve.size(); ++i)
            if (gridCurve[i]->equals(*gridCurve[i - 1])) {
                delete gridCurve[i];
                gridCurve.erase(gridCurve.begin() + i);
                i--;
            }


        /* Create the vectorial representation of the Grid Curve */
        std::vector<long double> coordinates;
        for (Vector *vector: gridCurve) {
            for (long double coordinate: vector->getCoordinates())
                coordinates.push_back(coordinate);
            delete vector;
        }

        /* Padding of the Grid Curve */
        while (coordinates.size() < this->vectorSpaceDimensions)
            coordinates.push_back(Grid::LARGE_PADDING_NUMBER);

        /* Trim of the Grid Curve */
        while (coordinates.size() > this->vectorSpaceDimensions)
            coordinates.erase(coordinates.begin() + (coordinates.size() - 1));

        return new Point(curve, coordinates);
    }

}

unsigned int Grid::getDimensions() const {
    return this->dimensions;
}



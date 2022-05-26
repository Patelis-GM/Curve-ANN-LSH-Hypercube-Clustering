#include "Curve.h"
#include "../../Metric-Modules/Vector-Metric/VectorMetric.h"

const long double Curve::LARGE_PADDING_NUMBER = 10000.0;

/* Utility/Guard function to create a Curve of dimensions 1 or 2 */
Curve *Curve::createCurve(unsigned int dimensions) {

    if (dimensions != 1 && dimensions != 2)
        return nullptr;

    return new Curve(dimensions);
}

/* Utility/Guard function to create a named Curve of dimensions 1 or 2 */
Curve *Curve::createCurve(const std::string &id, unsigned int dimensions) {

    if (dimensions != 1 && dimensions != 2)
        return nullptr;

    return new Curve(id, dimensions);
}

Curve::Curve(unsigned int dimensions) {

    this->dimensions = dimensions;
    this->vectorialRepresentation = nullptr;
    this->shouldCalculateVectorialRepresentation = false;

}

Curve::Curve(const std::string &id, unsigned int dimensions) {

    this->id = id;
    this->dimensions = dimensions;
    this->vectorialRepresentation = nullptr;
    this->shouldCalculateVectorialRepresentation = false;
}

Curve::~Curve() {

    if (this->vectorialRepresentation != nullptr)
        delete this->vectorialRepresentation;

    for (Vector *vector: this->points)
        delete vector;
}

void Curve::insert(Vector *vector) {

    if (vector->getDimensions() == this->dimensions) {
        this->points.push_back(vector);
        this->shouldCalculateVectorialRepresentation = true;
    }
}

/* Utility function to filter a Curve according to an e  */
void Curve::filter(long double e) {

    if (e < 0 || this->dimensions > 2)
        return;

    /* Given a set of Points e.g {A,B,C,D,E} then one of the following scenarios will occur :
     * 1) B does not get removed and the next triplet to examine is {B,C,D}
     * 2) B does get removed and the next triplet to examine is {C,D,E} */
    for (long unsigned int i = 0; i < this->points.size(); ++i) {

        if ((i + 2) >= this->points.size())
            break;

        Vector *a = this->points[i];
        Vector *b = this->points[i + 1];
        Vector *c = this->points[i + 2];

        if (this->dimensions == 1 && std::abs(a->getCoordinates()[0] - b->getCoordinates()[0]) <= e && std::abs(b->getCoordinates()[0] - c->getCoordinates()[0]) <= e) {
            delete b;
            this->points.erase(this->points.begin() + (i + 1));
        }
        else if (this->dimensions == 2 && std::abs(a->getCoordinates()[1] - b->getCoordinates()[1]) <= e && std::abs(b->getCoordinates()[1] - c->getCoordinates()[1]) <= e) {
            delete b;
            this->points.erase(this->points.begin() + (i + 1));
        }

    }

    /* Guard loop to make sure that the Curve's complexity is always greater than or equal to 2 as the Continuous Frechet Distance library provided only works for Curves with complexity of at least 2 */
    while (this->getComplexity() < 2) {

        /* In case the Curve's dimensions are equal to 1 then insert a new Point in the following format {10000.0} */
        if (this->dimensions == 1)
            this->points.push_back(new Vector(std::vector<long double>(this->dimensions, Curve::LARGE_PADDING_NUMBER)));

        /* In case the Curve's dimensions are greater than or equal to 2 then insert a new Point in the following format {(x-of-latest-Point + 1.0), 10000.0, 10000.0, ... 10000.0} */
        else {
            std::vector<long double> coordinates;
            coordinates.push_back(this->points[this->getComplexity() - 1]->getCoordinates()[0] + 1.0);
            for (unsigned int i = 1; i < this->dimensions; ++i)
                coordinates.push_back(Curve::LARGE_PADDING_NUMBER);
            this->points.push_back(new Vector(coordinates));
        }
    }

    /* Curve's vectorial representation should be calculated again */
    this->shouldCalculateVectorialRepresentation = true;
}

Point *Curve::getVectorialRepresentation() {

    /* Calculate the Curve's vectorial representation if it has not been calculated, or if it should be calculated again */
    if (this->shouldCalculateVectorialRepresentation || this->vectorialRepresentation == nullptr) {
        this->calculateVectorialRepresentation();
        this->shouldCalculateVectorialRepresentation = false;
    }

    return this->vectorialRepresentation;
}

unsigned int Curve::getComplexity() const {
    return this->points.size();
}

unsigned int Curve::getDimensions() const {
    return this->dimensions;
}

const std::vector<Vector *> &Curve::getPoints() const {
    return this->points;
}

const std::string &Curve::getID() const {
    return this->id;
}

/* Utility operator defined for the provided Continuous-Frechet-Distance library */
Vector &Curve::operator[](unsigned int index) const {
    return *this->points[index];
}

/* Utility function to calculate a Curve's vectorial representation */
void Curve::calculateVectorialRepresentation() {

    /* In case the Curve's vectorial representation has been calculated in the past, delete the previous vectorial representation */
    if (this->vectorialRepresentation != nullptr)
        delete this->vectorialRepresentation;

    std::vector<long double> coordinates;

    /* For each Point in the Curve get its coordinates and insert them in the coordinates std::vector */
    for (Vector *point: this->points)
        for (long double coordinate: point->getCoordinates())
            coordinates.push_back(coordinate);


    this->vectorialRepresentation = new Point(this, coordinates);
}

/* Utility function to calculate a Curve's std::string representation */
std::string Curve::toString() const {

    std::string stringRepresentation = "[";

    for (long unsigned int i = 0; i < this->points.size(); ++i) {
        stringRepresentation += this->points[i]->toString();
        if (i < this->points.size() - 1)
            stringRepresentation += ",";
    }

    stringRepresentation += "]";

    return stringRepresentation;
}






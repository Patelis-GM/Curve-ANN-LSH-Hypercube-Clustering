#include "CurveLSHTable.h"
#include "../../../Utilities/Arithmetic/Arithmetic.h"

/* ----
 *
 * README :
 * When inserting a Curve say X the following procedure takes places :
 *
 * (1) X's Grid Curve say Y is calculated in the Grid class.
 * (2) Grid::snap returns the vectorial representation of Y say V
 * (3) V gets hashed and inserted in the corresponding bucket alongside its amplified hash value
 *
 * ---- */

CurveLSHTable::CurveLSHTable(unsigned int id, unsigned int dimensions, double norm, unsigned int window, unsigned int k, unsigned int size, unsigned int gridDimensions, long double delta, bool continuous) {

    this->id = id;
    this->size = size;

    /* Create an amplified hash function and a Grid */
    this->lshHashFunction = new LSHHashFunction(dimensions, norm, window, k);
    this->grid = Grid::createGrid(dimensions, gridDimensions, delta, continuous);

    /* See README above */
    this->buckets = new std::vector<std::pair<unsigned int, Point *>>[size];
}

CurveLSHTable::~CurveLSHTable() {

    delete this->grid;
    delete this->lshHashFunction;
    /* In each bucket of the CurveLSHTable delete each stored vectorial representation of the corresponding Grid Curve */
    for (unsigned int i = 0; i < this->size; ++i)
        for (std::pair<unsigned int, Point *> pair: this->buckets[i])
            delete pair.second;

    delete[] this->buckets;

}

std::pair<unsigned int, Point *> CurveLSHTable::hashCurve(Curve *curve) const {

    /* Get the vectorial representation V of a Curve's Grid Curve */
    Point *gridCurve = this->grid->snap(curve);

    /* Return (V-amplified-hash-value, V)*/
    return std::make_pair(this->lshHashFunction->hash(*gridCurve), gridCurve);

}

void CurveLSHTable::insert(Curve *const curve) {

    /* Get the (V-amplified-hash-value, V) pair */
    std::pair<unsigned int, Point *> hashPair = this->hashCurve(curve);

    /* Insert the (V-amplified-hash value, V) pair in the corresponding bucket */
    unsigned int position = Arithmetic::mod(hashPair.first, this->size);
    this->buckets[position].push_back(std::make_pair(hashPair.first, hashPair.second));

}


std::vector<Curve *> *CurveLSHTable::getSimilarCurves(Curve *const curve, int threshold) {

    std::vector<Curve *> *similarCurves = new std::vector<Curve *>;

    /* Get the (QV-amplified-hash-value, QV) pair of the query Curve */
    std::pair<unsigned int, Point *> queryPair = this->hashCurve(curve);

    /* Calculate the bucket Y (QV-amplified-hash-value mod table-size) of the given query Curve */
    unsigned int position = Arithmetic::mod(queryPair.first, this->size);

    /* For each pair (Vi-amplified-hash-value, Vi) in Y get the ones [(Vi)->getCurve()] that Vi-amplified-hash-value = QV-amplified-hash-value */
    for (std::pair<unsigned int, Point *> pair: this->buckets[position])
        if (pair.first == queryPair.first)
            similarCurves->push_back(pair.second->getCurve());

    /* If no Curves were found due to the fact that Y was not empty and no Vi-amplified-hash-value was equal to QV-amplified-hash-value */
    if (similarCurves->empty() && !this->buckets[position].empty()) {

        /* If the threshold defined in CurveLSH is equal to 0 get each available Curve */
        if (threshold == 0)
            for (std::pair<unsigned int, Point *> pair: this->buckets[position])
                similarCurves->push_back(pair.second->getCurve());

            /* Otherwise, get as many Curves as possible */
        else
            for (unsigned int i = 0; i < this->buckets[position].size() && threshold > 0; ++i) {
                similarCurves->push_back(this->buckets[position].at(i).second->getCurve());
                threshold--;
            }
    }


    /* Delete QV as it is no longer needed */
    delete queryPair.second;


    return similarCurves;

}

unsigned int CurveLSHTable::getGridDimensions() const {
    return this->grid->getDimensions();
}

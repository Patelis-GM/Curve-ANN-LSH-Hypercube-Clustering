#include "VectorLSHTable.h"
#include "../../../Utilities/Arithmetic/Arithmetic.h"

VectorLSHTable::VectorLSHTable(unsigned int id, unsigned int dimensions, double norm, unsigned int window, unsigned int k, unsigned int size) {

    this->id = id;
    this->size = size;

    /* Create an amplified hash function */
    this->lshHashFunction = new LSHHashFunction(dimensions, norm, window, k);

    /* Create |size| std::vector<std::pair<unsigned int,Point*>> which act as the buckets of the hash table
     * Each bucket stores the Points assigned to it and their corresponding amplified hash values */
    this->buckets = new std::vector<std::pair<unsigned int, Point *>>[size];
}

VectorLSHTable::~VectorLSHTable() {

    delete this->lshHashFunction;
    delete[] this->buckets;
}

unsigned int VectorLSHTable::hashPoint(const Point &point) {
    return this->lshHashFunction->hash(point);
}

void VectorLSHTable::insert(Point *const point) {

    /* Calculate the amplified hash value X of the given Point */
    unsigned int amplifiedHashValue = this->hashPoint(*point);

    /* Calculate the bucket Y (X mod table-size) that the given Point should be assigned to */
    unsigned int position = Arithmetic::mod(amplifiedHashValue, this->size);

    /* Assign the pair (X, Point*) to the corresponding bucket Y */
    this->buckets[position].push_back(std::make_pair(amplifiedHashValue, point));
}

std::vector<Point *> *VectorLSHTable::getSimilarPoints(Point *const point, int threshold) {


    std::vector<Point *> *similarPoints = new std::vector<Point *>;

    /* Calculate the amplified hash value X of the given Point */
    unsigned int amplifiedHashValue = this->hashPoint(*point);

    /* Calculate the bucket Y (X mod table-size) of the given Point */
    unsigned int position = Arithmetic::mod(amplifiedHashValue, this->size);

    /* For each pair (Xi,Point*i) in Y get the ones (Point*i) that Xi = X */
    for (std::pair<unsigned int, Point *> pair: this->buckets[position])
        if (pair.first == amplifiedHashValue)
            similarPoints->push_back(pair.second);

    /* If no Points were found due to the fact that Y was not empty and no Xi was equal to X */
    if (similarPoints->empty() && !this->buckets[position].empty()) {

        /* If the threshold defined in VectorLSH is equal to 0 get each available Point */
        if (threshold == 0)
            for (std::pair<unsigned int, Point *> pair: this->buckets[position])
                similarPoints->push_back(pair.second);

            /* Otherwise, get as many Points as possible */
        else
            for (unsigned int i = 0; i < this->buckets[position].size() && threshold > 0; ++i) {
                similarPoints->push_back(this->buckets[position].at(i).second);
                threshold--;
            }

    }

    return similarPoints;
}



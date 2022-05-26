#include "HypercubeHashFunction.h"

HypercubeHashFunction::HypercubeHashFunction(unsigned int dimensions, double norm, unsigned int window, unsigned int k) {

    this->k = k;

    /* Create k HypercubeFunctions (h_i) */
    this->hypercubeFunctions = new HypercubeFunction *[k];
    for (unsigned int i = 0; i < k; ++i)
        this->hypercubeFunctions[i] = new HypercubeFunction(dimensions, norm, window);

}

HypercubeHashFunction::~HypercubeHashFunction() {

    for (unsigned int i = 0; i < this->k; ++i)
        delete this->hypercubeFunctions[i];

    delete[] this->hypercubeFunctions;

}

std::string HypercubeHashFunction::hash(const Point &point) {

    std::string vertex;

    /* Calculate the k f_i(h_i(v_i)) values for the given Point and concatenate them */
    for (unsigned int i = 0; i < this->k; ++i)
        vertex += this->hypercubeFunctions[i]->hash(point);

    return vertex;
}

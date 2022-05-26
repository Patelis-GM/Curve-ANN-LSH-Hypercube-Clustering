#include <algorithm>
#include "DiscreteFrechet.h"
#include "../Vector-Metric/VectorMetric.h"

DiscreteFrechet::DiscreteFrechet() {}

DiscreteFrechet::~DiscreteFrechet() {}

/* Utility function to calculate the Discrete Frechet distance between 2 Curves according to the slides that we were provided with */
long double DiscreteFrechet::distance(const Curve &curveOne, const Curve &curveTwo) const {

    if (curveOne.getComplexity() == 0 || curveTwo.getComplexity() == 0)
        return CurveMetric::INVALID_PARAMETERS;

    long double **distances = new long double *[curveOne.getComplexity()];
    for (unsigned int i = 0; i < curveOne.getComplexity(); ++i)
        distances[i] = new long double[curveTwo.getComplexity()];

    distances[0][0] = VectorMetric::distance(curveOne[0], curveTwo[0], 2.0);


    for (unsigned int i = 0; i < curveOne.getComplexity(); ++i)
        for (unsigned int j = 0; j < curveTwo.getComplexity(); ++j) {

            if (i == 0 && j > 0)
                distances[i][j] = std::max(distances[i][j - 1], VectorMetric::distance(curveOne[i], curveTwo[j], 2.0));

            else if (i > 0 && j == 0)
                distances[i][j] = std::max(distances[i - 1][j], VectorMetric::distance(curveOne[i], curveTwo[j], 2.0));

            else if (i > 0 && j > 0)
                distances[i][j] = std::max(std::min(std::min(distances[i - 1][j], distances[i - 1][j - 1]), distances[i][j - 1]), VectorMetric::distance(curveOne[i], curveTwo[j], 2.0));
        }


    long double result = distances[curveOne.getComplexity() - 1][curveTwo.getComplexity() - 1];

    for (unsigned int i = 0; i < curveOne.getComplexity(); ++i)
        delete[] distances[i];

    delete[] distances;

    return result;
}

/* Utility function to calculate the Discrete Frechet optimal traversal of 2 Curves according to the slides that we were provided with */
std::vector<std::pair<Vector *, Vector *>> *DiscreteFrechet::getOptimalTraversal(const Curve &curveOne, const Curve &curveTwo) const {

    if (curveOne.getComplexity() == 0 || curveTwo.getComplexity() == 0)
        return nullptr;

    long double **distances = new long double *[curveOne.getComplexity()];
    for (unsigned int i = 0; i < curveOne.getComplexity(); ++i)
        distances[i] = new long double[curveTwo.getComplexity()];

    distances[0][0] = VectorMetric::distance(curveOne[0], curveTwo[0], 2.0);

    for (unsigned int i = 0; i < curveOne.getComplexity(); ++i)
        for (unsigned int j = 0; j < curveTwo.getComplexity(); ++j) {

            if (i == 0 && j > 0)
                distances[i][j] = std::max(distances[i][j - 1], VectorMetric::distance(curveOne[i], curveTwo[j], 2.0));

            else if (i > 0 && j == 0)
                distances[i][j] = std::max(distances[i - 1][j], VectorMetric::distance(curveOne[i], curveTwo[j], 2.0));

            else if (i > 0 && j > 0)
                distances[i][j] = std::max(std::min(std::min(distances[i - 1][j], distances[i - 1][j - 1]), distances[i][j - 1]), VectorMetric::distance(curveOne[i], curveTwo[j], 2.0));
        }


    std::vector<std::pair<Vector *, Vector *>> *optimalTraversal = new std::vector<std::pair<Vector *, Vector *>>;

    optimalTraversal->push_back(std::make_pair<Vector *, Vector *>(&(curveOne[curveOne.getComplexity() - 1]), &(curveTwo[curveTwo.getComplexity() - 1])));

    unsigned int i = curveOne.getComplexity() - 1, j = curveTwo.getComplexity() - 1;

    while (i > 0 || j > 0) {

        unsigned int iMin = i;
        unsigned int jMin = j;
        long double minimumDistance = distances[i][j];

        if (j > 0 && distances[i][j - 1] <= minimumDistance) {
            minimumDistance = distances[i][j - 1];
            iMin = i;
            jMin = j - 1;
        }

        if (i > 0 && distances[i - 1][j] <= minimumDistance) {
            minimumDistance = distances[i - 1][j];
            iMin = i - 1;
            jMin = j;
        }

        if (i > 0 && j > 0 && distances[i - 1][j - 1] <= minimumDistance) {
            iMin = i - 1;
            jMin = j - 1;
        }

        i = iMin;
        j = jMin;

        optimalTraversal->push_back(std::make_pair<Vector *, Vector *>(&curveOne[iMin], &curveTwo[jMin]));
    }

    std::reverse(optimalTraversal->begin(), optimalTraversal->end());


    for (unsigned int k = 0; k < curveOne.getComplexity(); ++k)
        delete[] distances[k];

    delete[] distances;

    return optimalTraversal;

}




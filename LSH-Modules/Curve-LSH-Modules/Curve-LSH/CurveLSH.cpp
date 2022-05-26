#include <unordered_map>
#include <chrono>
#include "CurveLSH.h"

CurveLSH::CurveLSH(unsigned int dimensions, long double norm, unsigned int k, unsigned int w, unsigned int L, unsigned int hashTableSize, int threshold, unsigned int gridDimensions, long double delta, bool continuous) : CurveMethod(dimensions, norm, k, w) {

    /* Create L hash tables */
    for (unsigned int i = 0; i < L; ++i)
        this->curveLSHTables.push_back(new CurveLSHTable(i, this->dimensions, this->norm, this->w, this->k, hashTableSize, gridDimensions, delta, continuous));

    /* CurveLSH::threshold defines the maximum number of Curves that will be examined when using CurveLSH::getApproximateNearestNeighbor/CurveLSH::rangeSearch
     * In case CurveLSH::threshold = 0 then ALL eligible Curves will be examined  */
    this->threshold = threshold;
    if (this->threshold < 0)
        this->threshold = 0;
}

CurveLSH::~CurveLSH() {

    for (CurveLSHTable *curveLSHTable: this->curveLSHTables)
        delete curveLSHTable;
}

void CurveLSH::insertCurve(Curve *const curve) {

    /* Insert the given Curve to the CurveLSH's |L| hash tables as long as :
     * (1) Grid's dimensions = Curve's dimensions
     * (2) Curve's complexity > 1 due to the fact that the provided Continuous Frechet Distance library only works for Curves of complexity of at least 2
     * (3) See README file - section CurveLSH */
    if (curve->getDimensions() == this->getGridDimensions() && curve->getComplexity() > 1 && curve->getVectorialRepresentation()->getDimensions() <= this->dimensions)
        for (unsigned int i = 0; i < this->curveLSHTables.size(); ++i)
            this->curveLSHTables[i]->insert(curve);
}

std::tuple<Curve *, long double, double> CurveLSH::getApproximateNearestNeighbor(Curve *const queryCurve, CurveMetric *curveMetric) {

    if (queryCurve->getDimensions() == this->getGridDimensions() && queryCurve->getComplexity() > 1 && queryCurve->getVectorialRepresentation()->getDimensions() <= this->dimensions) {

        /* Data structure to check if a Curve has already been examined */
        std::unordered_map<Curve *, bool> curvesConsidered;

        int totalCurvesConsidered = 0;
        long double minDistance = std::numeric_limits<long double>::max();
        Curve *nearestNeighbor = nullptr;

        /* Start the clock */
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        for (CurveLSHTable *curveLSHTable: this->curveLSHTables) {


            if ((this->threshold > 0 && totalCurvesConsidered >= this->threshold) || minDistance == 0.0)
                break;

            /* See CurveLSHTable::getSimilarCurves for more */
            int curvesToGet;
            if (this->threshold == 0)
                curvesToGet = 0;
            else if ((this->threshold / this->curveLSHTables.size()) > 0)
                curvesToGet = this->threshold / this->curveLSHTables.size();
            else
                curvesToGet = 1;

            std::vector<Curve *> *similarCurves = curveLSHTable->getSimilarCurves(queryCurve, curvesToGet);

            for (Curve *similarCurve: *similarCurves) {

                /* Check if the similar Curve has already been examined  */
                if (curvesConsidered.find(similarCurve) == curvesConsidered.end()) {

                    long double distanceToQueryCurve = curveMetric->distance(*queryCurve, *similarCurve);
                    if (distanceToQueryCurve < minDistance) {
                        minDistance = distanceToQueryCurve;
                        nearestNeighbor = similarCurve;
                    }

                    curvesConsidered[similarCurve] = true;
                    totalCurvesConsidered++;


                    if ((this->threshold > 0 && totalCurvesConsidered >= this->threshold) || minDistance == 0.0)
                        break;
                }

            }

            delete similarCurves;
        }

        /* Stop the clock */
        std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();

        auto duration = finish - start;

        return std::make_tuple(nearestNeighbor, minDistance, std::chrono::duration<double, std::milli>(duration).count());
    }
    else
        return std::make_tuple(nullptr, -1.0, -1.0);

}

std::vector<Curve *> *CurveLSH::rangeSearch(Curve *const queryCurve, long double R, CurveMetric *curveMetric) {


    std::vector<Curve *> *rangeSearchNeighbors = new std::vector<Curve *>;

    if (queryCurve->getDimensions() == this->getGridDimensions() && queryCurve->getComplexity() > 1 && queryCurve->getVectorialRepresentation()->getDimensions() <= this->dimensions) {

        std::unordered_map<Curve *, bool> curvesConsidered;

        int totalCurvesConsidered = 0;

        for (CurveLSHTable *curveLSHTable: this->curveLSHTables) {

            if (this->threshold > 0 && totalCurvesConsidered >= this->threshold)
                break;

            /* See CurveLSHTable::getSimilarCurves for more */
            int curvesToGet;
            if (this->threshold == 0)
                curvesToGet = 0;
            else if ((this->threshold / this->curveLSHTables.size()) > 0)
                curvesToGet = this->threshold / this->curveLSHTables.size();
            else
                curvesToGet = 1;

            std::vector<Curve *> *similarCurves = curveLSHTable->getSimilarCurves(queryCurve, curvesToGet);

            for (Curve *similarCurve: *similarCurves) {

                /* Check if the similar Curve has already been considered */
                if (curvesConsidered.find(similarCurve) == curvesConsidered.end()) {

                    long double distance = curveMetric->distance(*queryCurve, *similarCurve);

                    if (distance <= R)
                        rangeSearchNeighbors->push_back(similarCurve);

                    curvesConsidered[similarCurve] = true;
                    totalCurvesConsidered++;


                    if (this->threshold > 0 && totalCurvesConsidered >= this->threshold)
                        break;

                }

            }

            delete similarCurves;
        }

    }

    return rangeSearchNeighbors;
}

unsigned int CurveLSH::getGridDimensions() const {
    return this->curveLSHTables[0]->getGridDimensions();
}

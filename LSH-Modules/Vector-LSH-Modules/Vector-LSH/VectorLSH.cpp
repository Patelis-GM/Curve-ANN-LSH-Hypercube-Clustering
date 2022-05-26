#include <chrono>
#include <unordered_map>
#include "VectorLSH.h"

VectorLSH::VectorLSH(unsigned int dimensions, long double norm, unsigned int k, unsigned int w, unsigned int L, unsigned int hashTableSize, int threshold) : VectorMethod(dimensions, norm, k, w) {

    /* Create L hash tables */
    for (unsigned int i = 0; i < L; ++i)
        this->vectorLSHTables.push_back(new VectorLSHTable(i, this->dimensions, this->norm, this->w, this->k, hashTableSize));

    /* VectorLSH::threshold defines the maximum number of Points that will be examined when using VectorLSH::getApproximateNearestNeighbor/VectorLSH::rangeSearch
     * In case VectorLSH::threshold = 0 then ALL eligible Points will be examined  */
    this->threshold = threshold;
    if (this->threshold < 0)
        this->threshold = 0;
}

VectorLSH::~VectorLSH() {

    for (VectorLSHTable *vectorLSHTable: this->vectorLSHTables)
        delete vectorLSHTable;
}

void VectorLSH::insertPoint(Point *const point) {

    /* Insert the given Point to the VectorLSH's |L| hash tables */
    if (point->getDimensions() == this->dimensions)
        for (unsigned int i = 0; i < this->vectorLSHTables.size(); ++i)
            this->vectorLSHTables[i]->insert(point);
}

std::vector<Point *> *VectorLSH::rangeSearch(Point *const queryPoint, long double R, VectorMetric *vectorMetric) {

    std::vector<Point *> *rangeSearchNeighbors = new std::vector<Point *>;

    if (queryPoint->getDimensions() == this->dimensions) {

        /* Data structure to check if a Point has already been examined */
        std::unordered_map<Point *, bool> pointsConsidered;

        int totalPointsConsidered = 0;

        for (VectorLSHTable *vectorLSHTable: this->vectorLSHTables) {

            if (this->threshold > 0 && totalPointsConsidered >= this->threshold)
                break;

            /* See VectorLSHTable::getSimilarPoints for more */
            int pointsToGet;
            if (this->threshold == 0)
                pointsToGet = 0;
            else if ((this->threshold / this->vectorLSHTables.size()) > 0)
                pointsToGet = this->threshold / this->vectorLSHTables.size();
            else
                pointsToGet = 1;


            std::vector<Point *> *similarPoints = vectorLSHTable->getSimilarPoints(queryPoint, pointsToGet);

            for (Point *similarPoint: *similarPoints) {

                /* Check if the similar Point has already been considered */
                if (pointsConsidered.find(similarPoint) == pointsConsidered.end()) {

                    long double distance = vectorMetric->distance(*queryPoint, *similarPoint);

                    if (distance <= R)
                        rangeSearchNeighbors->push_back(similarPoint);

                    pointsConsidered[similarPoint] = true;
                    totalPointsConsidered++;


                    if (this->threshold > 0 && totalPointsConsidered >= this->threshold)
                        break;

                }
            }

            delete similarPoints;
        }
    }

    return rangeSearchNeighbors;
}

std::tuple<Point *, long double, double> VectorLSH::getApproximateNearestNeighbor(Point *const queryPoint, VectorMetric *vectorMetric) {

    if (queryPoint->getDimensions() == this->dimensions) {

        /* Data structure to check if a Point has already been examined */
        std::unordered_map<Point *, bool> pointsConsidered;

        int totalPointsConsidered = 0;
        long double minDistance = std::numeric_limits<long double>::max();
        Point *nearestNeighbor = nullptr;

        /* Start the clock */
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        for (VectorLSHTable *vectorLSHTable: this->vectorLSHTables) {


            if ((this->threshold > 0 && totalPointsConsidered >= this->threshold) || minDistance == 0.0)
                break;


            /* See VectorLSHTable::getSimilarPoints for more */
            int pointsToGet;
            if (this->threshold == 0)
                pointsToGet = 0;
            else if ((this->threshold / this->vectorLSHTables.size()) > 0)
                pointsToGet = this->threshold / this->vectorLSHTables.size();
            else
                pointsToGet = 1;


            std::vector<Point *> *similarPoints = vectorLSHTable->getSimilarPoints(queryPoint, pointsToGet);

            for (Point *similarPoint: *similarPoints) {

                /* Check if the similar Point has already been examined  */
                if (pointsConsidered.find(similarPoint) == pointsConsidered.end()) {

                    long double distanceToQueryPoint = vectorMetric->distance(*queryPoint, *similarPoint, this->norm);
                    if (distanceToQueryPoint < minDistance) {
                        minDistance = distanceToQueryPoint;
                        nearestNeighbor = similarPoint;
                    }

                    pointsConsidered[similarPoint] = true;
                    totalPointsConsidered++;


                    if ((this->threshold > 0 && totalPointsConsidered >= this->threshold) || minDistance == 0.0)
                        break;
                }

            }

            delete similarPoints;
        }

        /* Stop the clock */
        std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();

        auto duration = finish - start;

        return std::make_tuple(nearestNeighbor, minDistance, std::chrono::duration<double, std::milli>(duration).count());

    }
    else
        return std::make_tuple(nullptr, -1.0, -1.0);
}



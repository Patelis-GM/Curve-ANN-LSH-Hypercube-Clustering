#include <chrono>
#include "CurveClustering.h"
#include "CurveHashFunction.h"
#include "../../../Utilities/Random/Random.h"

CurveClustering::CurveClustering() {
    this->totalIterations = 0;
    this->vectorSpaceDimensions = 0;
    this->inputCurves = nullptr;
}

void CurveClustering::initializeCentroids(unsigned int totalClusters, CurveMetric *metric) {

    if (this->inputCurves != nullptr && !this->inputCurves->empty()) {

        unsigned int totalCurves = this->inputCurves->size();

        /* In case the total CurveClusters requested are equal or more than the Curves in the provided dataset */
        if (totalClusters >= totalCurves)
            totalClusters = std::max((unsigned long int) totalCurves - 1, (unsigned long int) 1);

        /* Array that stores if the corresponding Curve is a Centroid or not  */
        std::vector<bool> isCentroid(totalCurves, false);

        /* Array that stores the minimum squared Frechet distance of each Curve from a Centroid */
        std::vector<long double> squaredDistances(totalCurves, std::numeric_limits<long double>::max());

        /* Choose the first Centroid at random */
        unsigned int randomIndex = Random::randomInteger(0, totalCurves - 1);
        isCentroid[randomIndex] = true;

        this->clusters.push_back(new CurveCluster(this->inputCurves->at(randomIndex)));

        /* Initialize all the remaining Centroids */
        for (unsigned int i = 1; i < totalClusters; ++i) {

            /* Arrays that store the distances and indices of all candidate Centroids */
            std::vector<long double> candidatesDistances;
            std::vector<unsigned int> candidatesIndices;

            for (unsigned int j = 0; j < totalCurves; ++j) {

                /* If the current Curve is not a Centroid */
                if (!isCentroid[j]) {

                    /* Calculate the current Curve's Frechet distance from the most recently added Centroid */
                    CurveCluster *previousCluster = this->clusters[this->clusters.size() - 1];

                    long double distanceToCentroid = metric->distance(*this->inputCurves->at(j), *previousCluster->getCentroid());

                    distanceToCentroid *= distanceToCentroid;

                    /* If the calculated distance is the shortest one yet store it */
                    if (distanceToCentroid < squaredDistances[j])
                        squaredDistances[j] = distanceToCentroid;

                    candidatesIndices.push_back(j);
                    candidatesDistances.push_back(squaredDistances[j]);
                }
            }

            long double maxDistance = std::numeric_limits<long double>::min();

            for (long double distance: candidatesDistances)
                if (distance > maxDistance)
                    maxDistance = distance;


            /* Normalize the Curves' distances, so that the partial sums don't get too large */
            if (maxDistance > 0)
                for (unsigned int j = 0; j < candidatesDistances.size(); ++j)
                    candidatesDistances[j] /= maxDistance;


            /* Calculate all partial sums */
            for (unsigned int j = 1; j < candidatesDistances.size(); ++j)
                candidatesDistances[j] += candidatesDistances[j - 1];


            /* Choose a random number between 0 and the maximum partial sum, and find which candidate Curve it corresponds to */
            long double randomPartialSum = Random::randomLongDouble(0, candidatesDistances[candidatesDistances.size() - 1]);

            unsigned int index = this->binarySearch(candidatesDistances, randomPartialSum);

            /* Set the randomly chosen Curve as a Centroid and insert it into the set of Centroids */
            isCentroid[candidatesIndices[index]] = true;

            this->clusters.push_back(new CurveCluster(this->inputCurves->at(candidatesIndices[index])));

        }
    }
}

long double CurveClustering::calculateInitialRadius(CurveMetric *curveMetric) {

    long double minimumRadius = std::numeric_limits<long double>::max();

    /* Calculate every possible distance between the corresponding Centroid and the rest of the Centroids */
    for (unsigned int i = 0; i < this->clusters.size() - 1; ++i)
        for (unsigned int j = i + 1; j < this->clusters.size(); j++) {

            long double currentRadius = curveMetric->distance(*this->clusters[i]->getCentroid(), *this->clusters[j]->getCentroid());

            /*  Check if the distance calculated is the shortest one yet */
            if (currentRadius < minimumRadius)
                minimumRadius = currentRadius;
        }


    /* Return the minimum distance, multiplied by a coefficient */
    return 0.5 * minimumRadius;
}

void CurveClustering::reverseRangeSearch(long double radius, CurveMetric *curveMetric, CurveMethod *methodToUse) {

    std::unordered_map<CurveCluster *, std::vector<Curve *>> clustersCurves;
    std::unordered_map<Curve *, std::pair<CurveCluster *, long double>> curveToCluster;


    for (unsigned int i = 0; i < this->totalIterations; ++i) {

        curveToCluster.clear();
        clustersCurves.clear();

        /* Apply range search around the current CurveCluster's Centroid */
        for (CurveCluster *cluster: this->clusters) {

            std::vector<Curve *> *rangeSearchNeighbors = methodToUse->rangeSearch((cluster->getCentroid()), radius, curveMetric);

            for (Curve *curve: *rangeSearchNeighbors) {

                /* Calculate the distance between the Curve and the CurveCluster's Centroid */
                long double currentDistance = curveMetric->distance(*curve, *cluster->getCentroid());

                /* If the corresponding Curve hasn't been assigned to a CurveCluster yet, assign it to the current CurveCluster  */
                if (curveToCluster.find(curve) == curveToCluster.end())
                    curveToCluster.insert(std::make_pair(curve, std::make_pair(cluster, currentDistance)));


                    /* If the corresponding Curve has already been assigned to a CurveCluster assign it to the closest one */
                else if (currentDistance < curveToCluster[curve].second) {
                    curveToCluster[curve].second = currentDistance;
                    curveToCluster[curve].first = cluster;
                }
            }

            delete rangeSearchNeighbors;
        }

        /* Assign each Curve to the appropriate CurveCluster */
        for (std::pair<Curve *const, std::pair<CurveCluster *, long double>> pair: curveToCluster)
            clustersCurves[pair.second.first].push_back(pair.first);

        /* Update each CurveCluster's Centroid */
        for (std::pair<CurveCluster *, std::vector<Curve *>> pair: clustersCurves)
            pair.first->updateCentroid(pair.second, this->vectorSpaceDimensions);

        /* Double the radius */
        if (2 * radius > 0)
            radius *= 2.0;
    }

    /* Dynamically allocated array to use in Lloyd's algorithm */
    std::unordered_map<Curve *, bool> *assignedCurves = new std::unordered_map<Curve *, bool>;

    /* Assign each Curve to the appropriate CurveCluster and mark it so that it won't get assigned to a CurveCluster during Lloyd's algorithm */
    for (std::pair<Curve *, std::pair<CurveCluster *, long double>> pair: curveToCluster) {
        pair.second.first->assignCurve(pair.first);
        assignedCurves->insert(std::make_pair(pair.first, true));
    }

    /* Apply LLoyd's method for the Curves that weren't assigned to any CurveCluster */
    this->Lloyd(assignedCurves, 1, curveMetric);

    delete methodToUse;
    delete assignedCurves;
}

void CurveClustering::Lloyd(std::unordered_map<Curve *, bool> *assignedCurves, unsigned int iterations, CurveMetric *curveMetric) {

    std::unordered_map<CurveCluster *, std::vector<Curve *>> clustersCurves;

    for (unsigned int i = 0; i < iterations; ++i) {

        clustersCurves.clear();

        for (Curve *curve: *this->inputCurves) {

            /* If the current Curve hasn't been assigned to a CurveCluster yet */
            if (assignedCurves->find(curve) == assignedCurves->end()) {

                /* Calculate the best-fitting CurveCluster X for the Curve.
                 * X is the one whose Centroid has the minimum distance from the Curve */
                CurveCluster *bestCluster = this->clusters[0];
                long double minimumDistance = std::numeric_limits<long double>::max();

                for (CurveCluster *cluster: this->clusters) {

                    long double distanceToCentroid = curveMetric->distance(*cluster->getCentroid(), *curve);

                    if (distanceToCentroid < minimumDistance) {
                        minimumDistance = distanceToCentroid;
                        bestCluster = cluster;
                    }
                }

                clustersCurves[bestCluster].push_back(curve);
            }
        }


        /* Update each CurveCluster's Centroid */
        for (std::pair<CurveCluster *, std::vector<Curve *>> pair: clustersCurves)
            pair.first->updateCentroid(pair.second, this->vectorSpaceDimensions);

    }


    /* Assign each Curve to the appropriate CurveCluster */
    for (std::pair<CurveCluster *, std::vector<Curve *>> pair: clustersCurves)
        for (Curve *curve: pair.second)
            pair.first->assignCurve(curve);

}

std::vector<long double> *CurveClustering::averageSilhouettes(CurveMetric *metric) {

    if (this->clusters.size() == 0 || metric == nullptr)
        return nullptr;

    /* Unordered map to store any distance that has already been calculated between 2 Curves
     * CurveHashFunction is used to calculate the hash value of the corresponding pair */
    std::unordered_map<std::pair<Curve *, Curve *>, long double, CurveHashFunction> previouslyCalculatedDistances;

    /* Dynamically allocated array that stores the average Silhouette value of each CurveCluster and the average Silhouette value of Curves in the dataset */
    std::vector<long double> *averageSilhouettes = new std::vector<long double>(this->clusters.size() + 1, 0.0);

    /* Array that stores which Clusters have a valid average Silhouette and which don't */
    std::vector<bool> hasValidAverageSilhouette(this->clusters.size(), true);

    unsigned int validCurves = 0;

    /* Calculate each Curve's Silhouette value */
    for (unsigned int i = 0; i < this->clusters.size(); ++i) {

        CurveCluster *currentCluster = this->clusters.at(i);

        unsigned int clusterSize = currentCluster->getAssignedCurves()->size();

        /* Consider each CurveCluster that has at least 2 Curves, otherwise the Silhouette value of its Curves (if any) cannot be defined */
        if (clusterSize > 1) {

            /* Examine the CurveCluster's Curves */
            for (Curve *curve: *currentCluster->getAssignedCurves()) {

                /* Find the second-closest CurveCluster to the current Curve */
                long double minimumDistance = std::numeric_limits<long double>::max();
                unsigned int secondBestCluster = i;

                for (unsigned int j = 0; j < this->clusters.size(); ++j)

                    if (j != i && !this->clusters.at(j)->getAssignedCurves()->empty()) {

                        long double currentDistance = metric->distance(*curve, *this->clusters.at(j)->getCentroid());

                        if (currentDistance < minimumDistance) {
                            minimumDistance = currentDistance;
                            secondBestCluster = j;
                        }
                    }


                /* If the rest of the Clusters were empty, then the b(i) value of the Curve cannot be defined */
                if (secondBestCluster == i) {
                    hasValidAverageSilhouette.at(i) = false;
                    break;
                }

                /* Calculate the b(i) value of the current Curve.
                 * b(i) is the average distance between the corresponding Curve and all Curves that belong to the second-best CurveCluster */
                unsigned int secondBestClusterSize = this->clusters.at(secondBestCluster)->getAssignedCurves()->size();
                long double curvesBi = 0.0;

                for (Curve *clusterCurve: *this->clusters.at(secondBestCluster)->getAssignedCurves()) {

                    /* Pair <clusterCurve, curve> to check if the distance between them was calculated recently */
                    std::pair<Curve *, Curve *> curvePair = std::make_pair(clusterCurve, curve);

                    /* Define an iterator to avoid re-hashing the reversed pair at "else" section  */
                    std::unordered_map<std::pair<Curve *, Curve *>, long double, CurveHashFunction>::iterator pairIterator = previouslyCalculatedDistances.find(curvePair);

                    /* In case the distance between 'curve' and 'clusterCurve' wasn't calculated recently */
                    if (pairIterator == previouslyCalculatedDistances.end()) {

                        /* Calculate the distance between them and store it */
                        long double curveDistance = metric->distance(*curve, *clusterCurve);
                        previouslyCalculatedDistances.insert(std::make_pair(std::make_pair(curve, clusterCurve), curveDistance));
                        curvesBi += curveDistance;

                    }

                        /* Fetch the recently calculated distance */
                    else
                        curvesBi += pairIterator->second;
                }

                curvesBi /= (long double) secondBestClusterSize;


                /* Calculate the a(i) value of the current Curve
                 * a(i) is the average distance between the corresponding Curve and all Curves that belong to its CurveCluster */
                long double curvesAi = 0.0;
                for (Curve *clusterCurve: *currentCluster->getAssignedCurves())
                    if (clusterCurve != curve) {

                        /* Pair <clusterCurve, Curve> to check if the distance between them was calculated recently */
                        std::pair<Curve *, Curve *> curvePair = std::make_pair(clusterCurve, curve);

                        /* Define an iterator to avoid re-hashing the reversed pair at "else" section  */
                        std::unordered_map<std::pair<Curve *, Curve *>, long double, CurveHashFunction>::iterator pairIterator = previouslyCalculatedDistances.find(curvePair);

                        /* In case the distance between 'Curve' and 'clusterCurve' wasn't calculated recently */
                        if (pairIterator == previouslyCalculatedDistances.end()) {

                            /* Calculate the distance between them and store it */
                            long double curveDistance = metric->distance(*curve, *clusterCurve);

                            previouslyCalculatedDistances.insert(std::make_pair(std::make_pair(curve, clusterCurve), curveDistance));
                            curvesAi += curveDistance;

                        }

                            /* Fetch the recently calculated distance */
                        else
                            curvesAi += pairIterator->second;

                    }

                curvesAi /= ((long double) clusterSize - 1.0);


                /* Calculate the Silhouette value of the current Curve by combining the calculated a(i) and b(i) values */
                long double currentSilhouette = (curvesBi - curvesAi) / std::max(curvesBi, curvesAi);
                averageSilhouettes->at(i) += currentSilhouette;
                ++validCurves;
                averageSilhouettes->at(averageSilhouettes->size() - 1) += currentSilhouette;
            }
        }
        else
            hasValidAverageSilhouette.at(i) = false;

    }

    /* 2.0 works as an error value in case the average Silhouette value of a CurveCluster or of all Curves in the dataset cannot be defined  */

    /* Check which Clusters have a valid average Silhouette value.
     * A CurveCluster has a valid average Silhouette value only if it contains at least 2 Curves and the rest of the Clusters aren't empty */
    for (unsigned int i = 0; i < this->clusters.size(); ++i) {

        if (hasValidAverageSilhouette.at(i))
            averageSilhouettes->at(i) /= this->clusters[i]->getAssignedCurves()->size();
        else
            averageSilhouettes->at(i) = 2.0;
    }

    /* Insert the average Silhouette value of all Curves in the dataset */
    if (validCurves > 0)
        averageSilhouettes->at(averageSilhouettes->size() - 1) /= (long double) validCurves;
    else
        averageSilhouettes->at(averageSilhouettes->size() - 1) = 2.0;

    return averageSilhouettes;
}

CurveClusteringResults *CurveClustering::apply(unsigned int kMeans, CurveMethod *curveMethod, CurveMetric *metric, std::vector<Curve *> *inputCurves, unsigned int totalIterations, unsigned int vectorSpaceDimensions, bool silhouette) {

    /* Check if the given input arguments are valid */
    if (metric == nullptr || kMeans == 0 || inputCurves == nullptr || inputCurves->empty() || totalIterations == 0 || vectorSpaceDimensions == 0)
        return nullptr;

    this->inputCurves = inputCurves;
    this->totalIterations = totalIterations;
    this->vectorSpaceDimensions = vectorSpaceDimensions;

    /* In case of a previous execution */
    if (!this->clusters.empty())
        for (CurveCluster *cluster: this->clusters)
            delete cluster;

    if (curveMethod != nullptr) {

        /* Start the clock */
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::cout << "\n> Initializing Centroids..." << std::endl;

        /* Initialize each Centroid */
        this->initializeCentroids(kMeans, metric);

        std::cout << "\n> Applying Reverse Range Search method..." << std::endl;

        /* Calculate the radius needed by Reverse Range Search and apply the algorithm */
        long double radius = this->calculateInitialRadius(metric);
        this->reverseRangeSearch(radius, metric, curveMethod);

        /* Stop the clock */
        std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();
        auto duration = finish - start;
        double totalTime = std::chrono::duration<double, std::milli>(duration).count() / 1000.0;


        std::vector<long double> *averageSilhouettes = nullptr;
        if (silhouette) {
            std::cout << "\n> Calculating Silhouette..." << std::endl;

            /* Measure the CurveClustering method's accuracy by using the Silhouette metric */
            averageSilhouettes = this->averageSilhouettes(metric);
        }

        CurveClusteringResults *curveClusteringResults = new CurveClusteringResults(totalTime, this->clusters, averageSilhouettes);

        return curveClusteringResults;
    }
    else {

        /* Start the clock */
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::cout << "\n> Initializing Centroids..." << std::endl;

        /* Initialize each Centroid */
        this->initializeCentroids(kMeans, metric);

        std::cout << "\n> Applying Lloyd's method..." << std::endl;

        /* Data structure to check if a Curve has been assigned to a CurveCluster */
        std::unordered_map<Curve *, bool> *lloydMap = new std::unordered_map<Curve *, bool>;
        this->Lloyd(lloydMap, this->totalIterations, metric);
        delete lloydMap;


        std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();
        auto duration = finish - start;
        double totalTime = std::chrono::duration<double, std::milli>(duration).count() / 1000.0;

        std::vector<long double> *averageSilhouettes = nullptr;
        if (silhouette) {
            std::cout << "\n> Calculating Silhouette..." << std::endl;
            /* Measure the CurveClustering method's accuracy by using the Silhouette metric */
            averageSilhouettes = this->averageSilhouettes(metric);
        }

        CurveClusteringResults *curveClusteringResults = new CurveClusteringResults(totalTime, this->clusters, averageSilhouettes);

        return curveClusteringResults;

    }
}

/* Modified binary search to use in CurveClustering::initializeCentroids */
unsigned int CurveClustering::binarySearch(const std::vector<long double> &array, long double number) {

    bool indexFound = false;
    unsigned int index = array.size() / 2;
    unsigned int remainder = array.size() - index - 1;


    while (!indexFound) {

        if (number == array[index])
            indexFound = true;

        else if (number < array[index]) {

            if (index == 0 || number > array[index - 1])
                indexFound = true;

            else {
                remainder = index - index / 2 - 1;
                index = index / 2;
            }
        }
        else {
            index = index + remainder / 2 + remainder % 2;
            remainder = remainder / 2;
        }
    }

    return index;
}

CurveClustering::~CurveClustering() {

    for (CurveCluster *curveCluster: clusters)
        delete curveCluster;
}

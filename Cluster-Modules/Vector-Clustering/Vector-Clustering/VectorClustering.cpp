#include <chrono>
#include <iostream>
#include "VectorHashFunction.h"
#include "VectorClustering.h"
#include "../../../Utilities/Random/Random.h"
#include "../../../Utilities/Arithmetic/Arithmetic.h"

VectorClustering::VectorClustering() {
    this->totalIterations = 0;
    this->inputPoints = nullptr;
}

VectorClustering::~VectorClustering() {

    for (VectorCluster *cluster: this->clusters)
        delete cluster;
}

void VectorClustering::initializeCentroids(unsigned int totalClusters, VectorMetric *vectorMetric) {

    /* In case total Clusters requested are more than the provided dataset */
    if (totalClusters >= this->inputPoints->size())
        totalClusters = std::max(this->inputPoints->size() - 1, (unsigned long int) 1);

    unsigned int dimensions = this->inputPoints->at(0)->getDimensions();
    unsigned int totalPoints = this->inputPoints->size();

    /* Array that stores if the corresponding Point is a Centroid or not  */
    std::vector<bool> isCentroid(totalPoints, false);

    /* Array that stores the minimum squared distance of each Curve from a Centroid */
    std::vector<long double> squaredDistances(totalPoints, std::numeric_limits<long double>::max());

    /* Choose the first Centroid at random */
    unsigned int randomIndex = Random::randomInteger(0, totalPoints - 1);
    isCentroid[randomIndex] = true;

    this->clusters.push_back(new VectorCluster(dimensions, *(this->inputPoints->at(randomIndex))));

    /* Initialize all the remaining Centroids */
    for (unsigned int i = 1; i < totalClusters; ++i) {

        /* Arrays that store the distances and indices of all candidate Centroids */
        std::vector<long double> candidatesDistances;
        std::vector<unsigned int> candidatesIndices;

        for (unsigned int j = 0; j < totalPoints; ++j) {

            /* If the current Vector is not a Centroid */
            if (!isCentroid[j]) {

                /* Calculate the Vector's distance from the most recently added Centroid */
                VectorCluster *previousCluster = this->clusters[this->clusters.size() - 1];

                long double distanceToCentroid = vectorMetric->distance(*this->inputPoints->at(j), *previousCluster->getCentroid());
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


        /* Normalize the Vectors' distances, so that the partial sums don't get too large */
        if (maxDistance > 0)
            for (unsigned int j = 0; j < candidatesDistances.size(); ++j)
                candidatesDistances[j] /= maxDistance;


        /* Calculate all partial sums */
        for (unsigned int j = 1; j < candidatesDistances.size(); ++j)
            candidatesDistances[j] += candidatesDistances[j - 1];


        /* Choose a random number between 0 and the maximum partial sum, and find which candidate Vector it corresponds to */
        long double randomPartialSum = Random::randomLongDouble(0, candidatesDistances[candidatesDistances.size() - 1]);

        unsigned int index = this->binarySearch(candidatesDistances, randomPartialSum);

        /* Set the randomly chosen Vector as a Centroid */
        isCentroid[candidatesIndices[index]] = true;

        this->clusters.push_back(new VectorCluster(dimensions, *(this->inputPoints->at(candidatesIndices[index]))));

    }

}

long double VectorClustering::calculateInitialRadius(VectorMetric *vectorMetric) {

    long double minimumRadius = std::numeric_limits<long double>::max();

    /* Calculate every possible distance between the corresponding Centroid and the rest of the Centroids */
    for (unsigned int i = 0; i < this->clusters.size() - 1; ++i)
        for (unsigned int j = i + 1; j < this->clusters.size(); j++) {

            long double currentRadius = vectorMetric->distance(*this->clusters[i]->getCentroid(), *this->clusters[j]->getCentroid());

            /*  Check if the distance calculated is the shortest one yet */
            if (currentRadius < minimumRadius)
                minimumRadius = currentRadius;
        }


    /* Return the minimum distance, multiplied by a coefficient */
    return 0.5 * minimumRadius;
}

VectorClusteringResults *VectorClustering::apply(unsigned int kMeans, VectorMethod *vectorMethod, VectorMetric *vectorMetric, unsigned int totalIterations, std::vector<Point *> *inputPoints, bool silhouette) {


    if (kMeans == 0 || inputPoints == nullptr || totalIterations == 0 || inputPoints->empty() || vectorMetric == nullptr)
        return nullptr;


    this->inputPoints = inputPoints;
    this->totalIterations = totalIterations;

    /* In case of a previous execution */
    if (!this->clusters.empty())
        for (VectorCluster *cluster: this->clusters)
            delete cluster;



    /* Start the clock */
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    std::cout << "\n> Initializing Centroids..." << std::endl;

    /* Initialize each Centroid */
    this->initializeCentroids(kMeans, vectorMetric);

    /* Apply the appropriate Clustering method */
    if (vectorMethod == nullptr) {

        std::cout << "\n> Applying Lloyd's method..." << std::endl;

        /* Data structure to check if a Curve has been assigned to a VectorCluster */
        std::unordered_map<Point *, bool> *lloydMap = new std::unordered_map<Point *, bool>;
        this->Lloyd(lloydMap, this->totalIterations, vectorMetric);
        delete lloydMap;

    }
    else {
        std::cout << "\n> Applying Reverse Range Search method..." << std::endl;
        long double radius = calculateInitialRadius(vectorMetric);
        this->reverseRangeSearch(vectorMethod, vectorMetric, radius);
    }

    /* Stop the clock */
    std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();
    auto duration = finish - start;
    double totalTime = std::chrono::duration<double, std::milli>(duration).count() / 1000.0;


    std::vector<long double> *averageSilhouettes = nullptr;
    if (silhouette) {

        std::cout << "\n> Calculating Silhouette..." << std::endl;

        /* Measure the Clustering method's accuracy by using the Silhouette metric */
        averageSilhouettes = this->averageSilhouettes(vectorMetric);
    }

    return new VectorClusteringResults(totalTime, this->clusters, averageSilhouettes);
}

void VectorClustering::reverseRangeSearch(VectorMethod *methodToUse, VectorMetric *vectorMetric, long double radius) {

    std::unordered_map<VectorCluster *, std::vector<Point *>> clustersPoints;
    std::unordered_map<Point *, std::pair<VectorCluster *, long double>> pointToCluster;


    for (unsigned int i = 0; i < this->totalIterations; ++i) {

        pointToCluster.clear();
        clustersPoints.clear();

        /* Apply Range Search around the current VectorCluster's Centroid */
        for (VectorCluster *cluster: this->clusters) {

            std::vector<Point *> *rangeSearchNeighbors = methodToUse->rangeSearch((cluster->getCentroid()), radius, vectorMetric);

            for (Point *point: *rangeSearchNeighbors) {

                /* Calculate the distance between the Vector and the VectorCluster's Centroid */
                long double currentDistance = vectorMetric->distance(*point, *cluster->getCentroid());

                /* If the corresponding Vector hasn't been assigned to a VectorCluster yet, assign the Curve to the current VectorCluster  */
                if (pointToCluster.find(point) == pointToCluster.end())
                    pointToCluster.insert(std::make_pair(point, std::make_pair(cluster, currentDistance)));

                    /* If the corresponding Vector has already been assigned to a VectorCluster assign it to the closest one */
                else if (currentDistance < pointToCluster[point].second) {
                    pointToCluster[point].second = currentDistance;
                    pointToCluster[point].first = cluster;
                }
            }

            delete rangeSearchNeighbors;
        }

        /* Assign each Vector to the appropriate VectorCluster */
        for (std::pair<Point *const, std::pair<VectorCluster *, long double>> pair: pointToCluster)
            clustersPoints[pair.second.first].push_back(pair.first);

        /* Update each VectorCluster's Centroid */
        for (std::pair<VectorCluster *, std::vector<Point *>> pair: clustersPoints)
            pair.first->updateCentroid(pair.second);

        /* Double the radius */
        if (2 * radius > 0)
            radius *= 2.0;
    }

    /* Dynamically allocated array to use in Lloyd's algorithm */
    std::unordered_map<Point *, bool> *assignedPoints = new std::unordered_map<Point *, bool>;

    /* Assign each Vector to the appropriate VectorCluster and mark it so that it won't get assigned to a VectorCluster during Lloyd's algorithm */
    for (std::pair<Point *, std::pair<VectorCluster *, long double>> pair: pointToCluster) {
        pair.second.first->assignPoint(pair.first);
        assignedPoints->insert(std::make_pair(pair.first, true));
    }

    /* Apply LLoyd's method for the Curves that weren't assigned to any VectorCluster */
    this->Lloyd(assignedPoints, 1, vectorMetric);

    delete assignedPoints;
}

void VectorClustering::Lloyd(std::unordered_map<Point *, bool> *assignedPoints, unsigned int iterations, VectorMetric *vectorMetric) {

    std::unordered_map<VectorCluster *, std::vector<Point *>> clustersPoints;

    for (unsigned int i = 0; i < iterations; ++i) {

        clustersPoints.clear();

        for (Point *point: *this->inputPoints) {

            /* If the current Vector hasn't been assigned to a VectorCluster yet */
            if (assignedPoints->find(point) == assignedPoints->end()) {

                /* Calculate the best-fitting VectorCluster X for the Vector.
                 * X is the one whose Centroid has the minimum distance from the Vector */
                VectorCluster *bestCluster = this->clusters[0];
                long double minimumDistance = std::numeric_limits<long double>::max();

                for (VectorCluster *cluster: this->clusters) {

                    long double distanceToCentroid = vectorMetric->distance(*cluster->getCentroid(), *point);

                    if (distanceToCentroid < minimumDistance) {
                        minimumDistance = distanceToCentroid;
                        bestCluster = cluster;
                    }
                }

                clustersPoints[bestCluster].push_back(point);
            }
        }

        /* Update each VectorCluster's Centroid */
        for (std::pair<VectorCluster *, std::vector<Point *>> pair: clustersPoints)
            pair.first->updateCentroid(pair.second);
    }


    /* Assign each Vector to the appropriate VectorCluster */
    for (std::pair<VectorCluster *, std::vector<Point *>> pair: clustersPoints)
        for (Point *point: pair.second)
            pair.first->assignPoint(point);

}

std::vector<long double> *VectorClustering::averageSilhouettes(VectorMetric *vectorMetric) {

    if (this->clusters.size() == 0)
        return nullptr;

    /* Unordered map to store any distance that has already been calculated between 2 Curves
     * VectorHashFunction is used to calculate the hash value of the corresponding pair */
    std::unordered_map<std::pair<Point *, Point *>, long double, VectorHashFunction> previouslyCalculatedDistances;

    /* Dynamically allocated array that stores the average Silhouette value of each VectorCluster and the average Silhouette value of Curves in the dataset */
    std::vector<long double> *averageSilhouettes = new std::vector<long double>(this->clusters.size() + 1, 0.0);

    /* Array that stores which Clusters have a valid average Silhouette and which don't */
    std::vector<bool> hasValidAverageSilhouette(this->clusters.size(), true);

    unsigned int validPoints = 0;

    /* Calculate each Vector's Silhouette value */
    for (unsigned int i = 0; i < this->clusters.size(); ++i) {

        VectorCluster *currentCluster = this->clusters.at(i);

        unsigned int clusterSize = currentCluster->getAssignedPoints()->size();

        /* Consider each VectorCluster that has at least 2 Vectors, otherwise the Silhouette value of its Vectors (if any) cannot be defined */
        if (clusterSize > 1) {

            /* Examine the VectorCluster's Curves */
            for (Point *point: *currentCluster->getAssignedPoints()) {

                /* Find the second-closest VectorCluster to the current Vector */
                long double minimumDistance = std::numeric_limits<long double>::max();
                unsigned int secondBestCluster = i;

                for (unsigned int j = 0; j < this->clusters.size(); ++j)

                    if (j != i && !this->clusters.at(j)->getAssignedPoints()->empty()) {

                        long double currentDistance = vectorMetric->distance(*point, *this->clusters.at(j)->getCentroid());

                        if (currentDistance < minimumDistance) {
                            minimumDistance = currentDistance;
                            secondBestCluster = j;
                        }
                    }


                /* If the rest of the Clusters were empty, then the b(i) value of the Vector cannot be defined */
                if (secondBestCluster == i) {
                    hasValidAverageSilhouette.at(i) = false;
                    break;
                }

                /* Calculate the b(i) value of the current Vector.
                 * b(i) is the average distance between the corresponding Vector and all Vectors that belong to the second-best VectorCluster */
                unsigned int secondBestClusterSize = this->clusters.at(secondBestCluster)->getAssignedPoints()->size();
                long double pointsBi = 0.0;

                for (Point *clusterPoint: *this->clusters.at(secondBestCluster)->getAssignedPoints()) {

                    /* Pair <clusterPoint, point> to check if the distance between them was calculated recently */
                    std::pair<Point *, Point *> pointPair = std::make_pair(clusterPoint, point);

                    /* Define an iterator to avoid re-hashing the reversed pair at "else" section  */
                    std::unordered_map<std::pair<Point *, Point *>, long double, VectorHashFunction>::iterator pairIterator = previouslyCalculatedDistances.find(pointPair);

                    /* In case the distance between 'clusterPoint' and 'point' wasn't calculated recently */
                    if (pairIterator == previouslyCalculatedDistances.end()) {

                        /* Calculate the distance between them and store it */
                        long double pointDistance = vectorMetric->distance(*point, *clusterPoint);
                        previouslyCalculatedDistances.insert(std::make_pair(std::make_pair(point, clusterPoint), pointDistance));
                        pointsBi += pointDistance;

                    }

                        /* Fetch the recently calculated distance */
                    else
                        pointsBi += pairIterator->second;
                }

                pointsBi /= (long double) secondBestClusterSize;


                /* Calculate the a(i) value of the current Vector
                 * a(i) is the average distance between the corresponding Vector and all Curves that belong to its VectorCluster */
                long double pointsAi = 0.0;
                for (Point *clusterPoint: *currentCluster->getAssignedPoints())
                    if (clusterPoint != point) {

                        /* Pair <clusterPoint, point> to check if the distance between them was calculated recently */
                        std::pair<Point *, Point *> pointPair = std::make_pair(clusterPoint, point);

                        /* Define an iterator to avoid re-hashing the reversed pair at "else" section  */
                        std::unordered_map<std::pair<Point *, Point *>, long double, VectorHashFunction>::iterator pairIterator = previouslyCalculatedDistances.find(pointPair);

                        /* In case the distance between 'clusterPoint' and 'point' wasn't calculated recently */
                        if (pairIterator == previouslyCalculatedDistances.end()) {

                            /* Calculate the distance between them and store it */
                            long double pointDistance = vectorMetric->distance(*point, *clusterPoint);
                            previouslyCalculatedDistances.insert(std::make_pair(std::make_pair(point, clusterPoint), pointDistance));
                            pointsAi += pointDistance;

                        }

                            /* Fetch the recently calculated distance */
                        else
                            pointsAi += pairIterator->second;

                    }
                pointsAi /= ((long double) clusterSize - 1.0);


                /* Calculate the Silhouette value of the current Curve by combining the calculated a(i) and b(i) values */
                long double currentSilhouette = (pointsBi - pointsAi) / std::max(pointsBi, pointsAi);
                averageSilhouettes->at(i) += currentSilhouette;
                ++validPoints;
                averageSilhouettes->at(averageSilhouettes->size() - 1) += currentSilhouette;
            }
        }
        else
            hasValidAverageSilhouette.at(i) = false;

    }

    /* 2.0 works as an error value in case the average Silhouette value of a VectorCluster or of all Curves in the dataset cannot be defined  */

    /* Check which Clusters have a valid average Silhouette value.
     * A VectorCluster has a valid average Silhouette value only if it contains at least 2 Vectors and the rest of the Clusters aren't empty */
    for (unsigned int i = 0; i < this->clusters.size(); ++i) {

        if (hasValidAverageSilhouette.at(i))
            averageSilhouettes->at(i) /= this->clusters[i]->getAssignedPoints()->size();
        else
            averageSilhouettes->at(i) = 2.0;
    }

    /* Insert the average Silhouette value of all Curves in the dataset */
    if (validPoints > 0)
        averageSilhouettes->at(averageSilhouettes->size() - 1) /= (long double) validPoints;
    else
        averageSilhouettes->at(averageSilhouettes->size() - 1) = 2.0;

    return averageSilhouettes;
}

/* Modified binary search to use in VectorClustering::initializeCentroids */
unsigned int VectorClustering::binarySearch(const std::vector<long double> &array, long double number) {

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

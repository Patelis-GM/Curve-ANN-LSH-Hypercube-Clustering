#include <chrono>
#include <algorithm>
#include "Hypercube.h"
#include "../../Utilities/String/String.h"

Hypercube::Hypercube(unsigned int dimensions, double norm, unsigned int k, unsigned int w, unsigned int probes, unsigned int M) : VectorMethod(dimensions, norm, k, w) {

    this->probes = probes;
    this->M = M;
    this->hypercubeHashFunction = new HypercubeHashFunction(dimensions, norm, w, k);
}

Hypercube::~Hypercube() {

    for (std::pair<std::string, std::vector<Point *> *> pair: this->vertices)
        delete pair.second;

    delete this->hypercubeHashFunction;
}

void Hypercube::insertPoint(Point *const point) {

    if (point->getDimensions() == this->dimensions) {

        /* Calculate the Vertex of the given Point */
        std::string inputPointVertex = this->hypercubeHashFunction->hash(*point);

        /* If the Vertex calculated hasn't been encountered before then create an array for its Points */
        if (this->vertices.find(inputPointVertex) == this->vertices.end())
            this->vertices[inputPointVertex] = new std::vector<Point *>;

        /* Insert the Point given to the calculated Vertex's array */
        this->vertices[inputPointVertex]->push_back(point);
    }

}

/* Function to perform R-range-search given a Vector metric */
std::vector<Point *> *Hypercube::rangeSearch(Point *const queryPoint, long double R, VectorMetric *vectorMetric) {


    std::vector<Point *> *similarPoints = new std::vector<Point *>;

    if (queryPoint->getDimensions() == this->dimensions) {

        std::string queryPointVertex = this->hypercubeHashFunction->hash(*queryPoint);

        std::vector<std::string> *verticesToVisit = this->getVerticesToVisit(queryPointVertex);

        /* Variable to check if M Points have been considered */
        unsigned int totalPointsChecked = 0;

        for (const std::string &vertexToVisit: *verticesToVisit) {

            /* Get the Points of the appropriate Vertex */
            std::vector<Point *> *vertexPoints = this->vertices[vertexToVisit];

            /* For each Point calculate its distance to the Query Point and check if it's less than or equal to R */
            for (Point *vertexPoint: *vertexPoints) {

                long double distance = vectorMetric->distance(*queryPoint, *vertexPoint);

                if (distance <= R)
                    similarPoints->push_back(vertexPoint);

                totalPointsChecked++;

                /* If M Points have been considered then it's time to return */
                if (totalPointsChecked == this->M) {
                    delete verticesToVisit;
                    return similarPoints;
                }
            }
        }

        delete verticesToVisit;
    }

    return similarPoints;
}

/* Function to get the best approximate nearest neighbor given a Vector metric */
std::tuple<Point *, long double, double> Hypercube::getApproximateNearestNeighbor(Point *const queryPoint, VectorMetric *vectorMetric) {

    if (queryPoint->getDimensions() == this->dimensions) {

        std::tuple<Point *, long double, double> neighbor;

        unsigned int totalPointsChecked = 0;
        long double minDistance = std::numeric_limits<long double>::max();
        Point *nearestNeighbor = nullptr;

        /* Start the clock */
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::string queryPointVertex = this->hypercubeHashFunction->hash(*queryPoint);

        std::vector<std::string> *verticesToVisit = this->getVerticesToVisit(queryPointVertex);

        for (const std::string &vertexToVisit: *verticesToVisit) {

            if (totalPointsChecked == this->M || minDistance == 0.0)
                break;

            std::vector<Point *> *vertexPoints = this->vertices[vertexToVisit];

            for (Point *vertexPoint: *vertexPoints) {

                long double distance = vectorMetric->distance(*queryPoint, *vertexPoint);

                if (distance < minDistance) {
                    minDistance = distance;
                    nearestNeighbor = vertexPoint;
                }


                totalPointsChecked++;

                if (totalPointsChecked == this->M || minDistance == 0.0)
                    break;
            }
        }

        /* Stop the clock */
        std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();

        auto duration = finish - start;

        neighbor = std::make_tuple(nearestNeighbor, minDistance, std::chrono::duration<double, std::milli>(duration).count());

        delete verticesToVisit;

        return neighbor;
    }
    else
        return std::make_tuple(nullptr, -1.0, -1.0);
}

/* Utility function to get the Vertices that their Hamming Distance is equal to X given a Point's Vertex */
std::vector<std::string> *Hypercube::getVerticesWithHammingDistance(const std::string &pointVertex, unsigned int i) {

    std::vector<std::string> *appropriateVertices = new std::vector<std::string>;

    for (std::pair<std::string, std::vector<Point *> *> pair: this->vertices)
        if (String::hammingDistance(pointVertex, pair.first) == i)
            appropriateVertices->push_back(pair.first);

    return appropriateVertices;
}

/* Utility function to get the appropriate Vertices to consider/visit given a Point's Vertex */
std::vector<std::string> *Hypercube::getVerticesToVisit(const std::string &pointVertex) {

    std::vector<std::string> *verticesToVisit = new std::vector<std::string>;

    /* Say k is equal to 3 then the minimum-maximum Hamming Distances will be : 0-3 */
    for (unsigned int i = 0; i <= this->k; ++i) {

        /* Get the Vertices that their Hamming Distance is equal to i  */
        std::vector<std::string> *appropriateVertices = this->getVerticesWithHammingDistance(pointVertex, i);

        /* Sprinkle some randomness */
        std::default_random_engine defaultRandomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::shuffle(appropriateVertices->begin(), appropriateVertices->end(), defaultRandomEngine);

        for (long unsigned int j = 0; j < appropriateVertices->size(); ++j) {

            verticesToVisit->push_back(appropriateVertices->at(j));

            /* At most '-probes' Vertices will be considered/visited */
            if (verticesToVisit->size() == this->probes) {
                delete appropriateVertices;
                return verticesToVisit;
            }
        }

        delete appropriateVertices;
    }

    return verticesToVisit;
}





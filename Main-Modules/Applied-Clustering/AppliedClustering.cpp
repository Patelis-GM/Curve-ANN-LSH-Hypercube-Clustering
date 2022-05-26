#include "AppliedClustering.h"
#include "../../CMD-Modules/Clustering-Arguments/ClusteringArguments.h"
#include "../../Utilities/File/File.h"
#include "../../Method-Modules/Curve-Method/CurveMethod.h"
#include "../../Cluster-Modules/Curve-Clustering/Curve-Clustering/CurveClustering.h"
#include "../../Metric-Modules/Discrete-Frechet/DiscreteFrechet.h"
#include "../../Utilities/Parser/Parser.h"
#include "../../LSH-Modules/Curve-LSH-Modules/Curve-LSH/CurveLSH.h"
#include "../../Metric-Modules/LP-Norm/LPNorm.h"
#include "../../Cluster-Modules/Vector-Clustering/Vector-Clustering/VectorClustering.h"
#include "../../LSH-Modules/Vector-LSH-Modules/Vector-LSH/VectorLSH.h"
#include "../../Hypercube-Modules/Hypercube/Hypercube.h"
#include <iostream>
#include <fstream>

const long double AppliedClustering::norm = 2.0;

void AppliedClustering::appliedClustering(int argc, char **argv) {

    /* Parse CMD arguments */
    ClusteringArguments *clusteringArguments = ClusteringArguments::parseCMDArguments(argc, argv);

    if (clusteringArguments != nullptr) {

        /* Get Input file's path */
        std::string inputFilePath = clusteringArguments->getInputFilePath();

        /* Get configuration file's path */
        std::string configurationFilePath = clusteringArguments->getConfigurationFilePath();

        /* Get Output file's path */
        std::string outputFilePath = clusteringArguments->getOutputFilePath();
        if (outputFilePath.empty())
            outputFilePath = AppliedClustering::requestOutputFilePath();

        /* Get -update argument */
        std::string updateMethod = clusteringArguments->getUpdateMethod();

        /* Get -assignment argument */
        std::string assignmentMethod = clusteringArguments->getAssignmentMethod();

        /* Get -complete argument */
        bool complete = clusteringArguments->isComplete();

        /* Get -silhouette argument */
        bool silhouette = clusteringArguments->shouldCalculateSilhouette();

        /* Parse configuration file */
        ClusteringParameters *clusteringParameters = ClusteringParameters::parseConfigurationFile(configurationFilePath);

        /* In case the parsing of the cluster.conf file fails */
        if (clusteringParameters == nullptr) {
            delete clusteringArguments;
            return;
        }

        /* Get kMeans from Clustering Parameters */
        unsigned int kMeans = clusteringParameters->getKMeans();

        /* Set total iterations */
        unsigned int totalIterations = 10;

        /* Parse Curves in Input File as 2 or 1 dimensional */
        std::vector<Curve *> *inputCurves;
        if (updateMethod == "Mean-Frechet")
            inputCurves = Parser::parseCurvesFile(inputFilePath, 2, 1.0, 0);
        else
            inputCurves = Parser::parseCurvesFile(inputFilePath, 1, 1.0, 0);

        /* See Parser::parseCurvesFile for more */
        if (inputCurves->empty()) {
            delete clusteringArguments;
            delete clusteringParameters;
            delete inputCurves;
            std::cout << "Error : No valid Curves were found in '" << inputFilePath << "'" << std::endl;
            return;
        }

        /* Get the vectorial representations of the Curves */
        std::vector<Point *> vectorialCurves;
        AppliedClustering::getVectorialRepresentations(*inputCurves, vectorialCurves);

        /* Create a LP (L2) Vector metric */
        VectorMetric *vectorMetric = new LPNorm(AppliedClustering::norm);

        if (updateMethod == "Mean-Frechet") {

            /* Dimensions are equal to the dimensions of the Curves' vectorial representations */
            unsigned int dimensions = vectorialCurves[0]->getDimensions();

            /* Use Discrete-Frechet Metric */
            CurveMetric *curveMetric = new DiscreteFrechet();

            /* CurveMethod is nullptr for LLoyd's method */
            CurveMethod *curveMethod = nullptr;

            if (assignmentMethod == "LSH_Frechet") {

                /* CurveMethod's parameters initialization */
                long double norm = AppliedClustering::norm;

                unsigned int k = clusteringParameters->getLSHK();

                /* Optimal w calculation. Percentage argument is 0 here due to the fact the provided Input file is of a minimal size */
                unsigned int w = CurveMethod::calculateOptimalWindow(0.0, vectorialCurves, vectorMetric);

                unsigned int L = clusteringParameters->getL();

                /* (inputCurves->size() / x) + 1 in case (inputCurves->size() / x) == 0 */
                unsigned int hashTableSize = (inputCurves->size() / 8) + 1;

                /* (inputCurves->size() / x) + 1 in case (inputCurves->size() / x) == 0 */
                unsigned int threshold = (inputCurves->size() / 3) + 1;

                unsigned int gridDimensions = inputCurves->at(0)->getDimensions();

                unsigned int exponent = 5;

                long double delta = CurveMethod::calculateOptimalDelta(*inputCurves, gridDimensions, exponent);

                bool continuous = false;

                curveMethod = new CurveLSH(dimensions, norm, k, w, L, hashTableSize, threshold, gridDimensions, delta, continuous);

                for (Curve *curve: *inputCurves)
                    curveMethod->insertCurve(curve);
            }

            CurveClustering curveClustering;
            CurveClusteringResults *curveClusteringResults = curveClustering.apply(kMeans, curveMethod, curveMetric, inputCurves, totalIterations, dimensions, silhouette);
            AppliedClustering::printCurveClusteringResults(outputFilePath, assignmentMethod, curveClusteringResults, complete, silhouette);

            delete curveMetric;
            delete curveClusteringResults;

        }

        /* (updateMethod = "Mean-Vector") */
        else {

            /* vectorMethod is nullptr for LLoyd's method */
            VectorMethod *vectorMethod = nullptr;

            /* VectorLSH-Hypercube shared variables */
            unsigned int dimensions = vectorialCurves[0]->getDimensions();

            long double norm = AppliedClustering::norm;

            /* Optimal w calculation. Percentage argument is 0 here due to the fact the provided Input file is of a minimal size */
            unsigned int w = VectorMethod::calculateOptimalWindow(0.0, vectorialCurves, vectorMetric);

            if (assignmentMethod == "LSH") {

                unsigned int k = clusteringParameters->getLSHK();
                unsigned int L = clusteringParameters->getL();

                /* (inputCurves->size() / x) + 1 in case (inputCurves->size() / x) == 0 */
                unsigned int hashTableSize = (inputCurves->size() / 8) + 1;

                /* (inputCurves->size() / x) + 1 in case (inputCurves->size() / x) == 0 */
                unsigned int threshold = (inputCurves->size() / 3) + 1;

                vectorMethod = new VectorLSH(dimensions, norm, k, w, L, hashTableSize, threshold);

            }
            else if (assignmentMethod == "Hypercube") {
                unsigned int k = clusteringParameters->getHypercubeK();
                unsigned int probes = clusteringParameters->getProbes();
                unsigned int M = clusteringParameters->getM();
                vectorMethod = new Hypercube(dimensions, norm, k, w, probes, M);
            }

            if (vectorMethod != nullptr)
                for (Point *point: vectorialCurves)
                    vectorMethod->insertPoint(point);


            VectorClustering vectorClustering;
            VectorClusteringResults *vectorClusteringResults = vectorClustering.apply(kMeans, vectorMethod, vectorMetric, totalIterations, &vectorialCurves, silhouette);

            AppliedClustering::printVectorClusteringResults(outputFilePath, assignmentMethod, vectorClusteringResults, complete, silhouette);

            if (vectorMethod != nullptr)
                delete vectorMethod;

            delete vectorClusteringResults;
        }


        delete vectorMetric;
        for (Curve *curve: *inputCurves)
            delete curve;
        delete inputCurves;
        delete clusteringParameters;
        delete clusteringArguments;
    }
}

/* Utility function to get the vectorial representations of the Curves in inputCurves and insert them in vectorialRepresentations */
void AppliedClustering::getVectorialRepresentations(const std::vector<Curve *> &inputCurves, std::vector<Point *> &vectorialRepresentations) {

    for (Curve *curve: inputCurves)
        vectorialRepresentations.push_back(curve->getVectorialRepresentation());
}


/* Utility function to request the file path of an existing file */
std::string AppliedClustering::requestOutputFilePath() {

    std::string outputFilePath;

    if (outputFilePath.empty())
        do {
            std::cout << "\nProvide the path of the Output File : ";
            std::getline(std::cin, outputFilePath);
            std::cout << std::endl;
        } while (!File::canOpenFile(outputFilePath));


    return outputFilePath;
}

void AppliedClustering::printCurveClusteringResults(const std::string &fileName, const std::string &method, CurveClusteringResults *clusteringResults, bool complete, bool silhouette) {

    if (clusteringResults == nullptr)
        return;

    std::ofstream outputFile;

    outputFile.open(fileName);

    /* Output the appropriate CurveClustering method */
    outputFile << "Algorithm: ";

    if (method == "Classic")
        outputFile << "Lloyd's" << std::endl;
    else
        outputFile << "LSH_Frechet" << std::endl;

    /* For each CurveCluster, output its ID, size and Centroid's coordinates */
    unsigned int clusterID = 1;

    for (CurveCluster *cluster: clusteringResults->getClusters()) {

        outputFile << "\nCluster-" << clusterID << " {size: " << cluster->getAssignedCurves()->size() << ", centroid: " << cluster->getCentroid()->toString();

        if (complete) {
            outputFile << " - Item_IDs:";

            for (Curve *curve: *cluster->getAssignedCurves())
                outputFile << " " << curve->getID();
        }

        outputFile << "}" << std::endl;

        ++clusterID;
    }

    /* Output the duration of the CurveClustering */
    outputFile << "\nclustering_time: " << clusteringResults->getDuration() << " seconds" << std::endl;

    if (silhouette && clusteringResults->getAverageSilhouettes() != nullptr) {

        /* For each CurveCluster, output its average Silhouette value */
        outputFile << "\nSilhouette: [";

        unsigned int i;

        for (i = 0; i < clusteringResults->getAverageSilhouettes()->size() - 1; ++i) {

            if (clusteringResults->getAverageSilhouettes()->at(i) == 2.0)
                outputFile << "NaN" << ", ";
            else
                outputFile << (float) clusteringResults->getAverageSilhouettes()->at(i) << ", ";
        }

        /* Output the average Silhouette value of all Curves in dataset */
        if (clusteringResults->getAverageSilhouettes()->at(i) == 2.0)
            outputFile << "NaN]" << std::endl;
        else
            outputFile << (float) clusteringResults->getAverageSilhouettes()->at(i) << "]" << std::endl;
    }

    outputFile.close();
}

void AppliedClustering::printVectorClusteringResults(const std::string &fileName, const std::string &method, VectorClusteringResults *clusteringResults, bool complete, bool silhouette) {

    if (clusteringResults == nullptr)
        return;

    std::ofstream outputFile;

    outputFile.open(fileName);

    /* Output the appropriate VectorClustering method */
    outputFile << "Algorithm: ";

    if (method == "Classic")
        outputFile << "Lloyd's" << std::endl;

    else if (method == "LSH")
        outputFile << "Range Search LSH" << std::endl;

    else
        outputFile << "Range Search Hypercube" << std::endl;

    /* For each VectorCluster, output its ID, size and Centroid's coordinates */
    unsigned int clusterID = 1;


    for (VectorCluster *cluster: clusteringResults->getClusters()) {

        outputFile << "\nCluster-" << clusterID << " {size: " << cluster->getAssignedPoints()->size() << ", centroid: " << cluster->getCentroid()->toString();

        if (complete) {
            outputFile << " - Item_IDs:";
            for (Point *point: *cluster->getAssignedPoints())
                outputFile << " " << point->getCurve()->getID();
        }

        outputFile << "}" << std::endl;

        ++clusterID;
    }


    /* Output the duration of the VectorClustering */
    outputFile << "\nclustering_time: " << clusteringResults->getDuration() << " seconds" << std::endl;


    if (silhouette && clusteringResults->getAverageSilhouettes() != nullptr) {


        /* For each VectorCluster, output its average Silhouette value */
        outputFile << "\nSilhouette: [";

        unsigned int i;

        for (i = 0; i < clusteringResults->getAverageSilhouettes()->size() - 1; ++i) {

            if (clusteringResults->getAverageSilhouettes()->at(i) == 2.0)
                outputFile << "NaN" << ", ";
            else
                outputFile << (float) clusteringResults->getAverageSilhouettes()->at(i) << ", ";
        }

        /* Output the average Silhouette value of all Curves in dataset */
        if (clusteringResults->getAverageSilhouettes()->at(i) == 2.0)
            outputFile << "NaN]" << std::endl;
        else
            outputFile << (float) clusteringResults->getAverageSilhouettes()->at(i) << "]" << std::endl;
    }

    outputFile.close();
}

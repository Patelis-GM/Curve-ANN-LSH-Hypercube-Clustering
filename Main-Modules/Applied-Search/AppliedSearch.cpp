#include <iostream>
#include <vector>
#include <fstream>
#include "AppliedSearch.h"
#include "../../Utilities/File/File.h"
#include "../../Utilities/Parser/Parser.h"
#include "../../Hypercube-Modules/Hypercube/Hypercube.h"
#include "../../LSH-Modules/Vector-LSH-Modules/Vector-LSH/VectorLSH.h"
#include "../../Method-Modules/Curve-Method/CurveMethod.h"
#include "../../LSH-Modules/Curve-LSH-Modules/Curve-LSH/CurveLSH.h"
#include "../../Metric-Modules/Discrete-Frechet/DiscreteFrechet.h"
#include "../../Metric-Modules/Continuous-Frechet/ContinuousFrechet.h"
#include "../../Metric-Modules/LP-Norm/LPNorm.h"


const long double AppliedSearch::norm = 2.0;

/* Utility function to get the vectorial representations of the Curves in inputCurves and insert them in vectorialRepresentations */
void AppliedSearch::getVectorialRepresentations(const std::vector<Curve *> &inputCurves, std::vector<Point *> &vectorialRepresentations) {

    for (Curve *curve: inputCurves)
        vectorialRepresentations.push_back(curve->getVectorialRepresentation());
}

void AppliedSearch::appliedSearch(int argc, char **argv) {

    SearchArguments *searchArguments = SearchArguments::parseCMDArguments(argc, argv);

    if (searchArguments != nullptr) {

        /* Request the file path of an existing, not empty, and in Linux format file */
        std::string inputFilePath = searchArguments->getInputFilePath();
        if (inputFilePath.empty())
            inputFilePath = AppliedSearch::requestInputFilePath();

        /* Request algorithm */
        std::string algorithm = searchArguments->getAlgorithm();
        if (algorithm.empty())
            algorithm = AppliedSearch::requestAlgorithm(searchArguments);

        /* Request metric */
        std::string metric = searchArguments->getMetric();
        if (algorithm == "Frechet" && metric.empty())
            metric = AppliedSearch::requestFrechetMetric(searchArguments);

        /* Request the file path of an existing, not empty, and in Linux format file */
        std::string queryFilePath = searchArguments->getQueryFilePath();
        if (queryFilePath.empty())
            queryFilePath = AppliedSearch::requestQueryFilePath();

        /* Request the file path of an existing file */
        std::string outputFilePath = searchArguments->getOutputFilePath();
        if (outputFilePath.empty())
            outputFilePath = AppliedSearch::requestOutputFilePath();

        std::vector<Curve *> *inputCurves;
        std::vector<Point *> vectorialInputCurves;
        unsigned int w;
        unsigned int k = searchArguments->getK();
        unsigned int curveDimensions;
        unsigned int vectorDimensions;

        /* Create a new L2Norm object */
        VectorMetric *vectorMetric = new LPNorm(AppliedSearch::norm);


        if (algorithm == "LSH" || algorithm == "Hypercube") {

            curveDimensions = 1;

            std::cout << "\nParsing Input file : '" << inputFilePath << "'..." << std::endl;

            /* Parse the provided input file */
            inputCurves = Parser::parseCurvesFile(inputFilePath, curveDimensions, 1.0, 0);

            /* See Parser::parseCurvesFile for more  */
            if (inputCurves->empty()) {
                delete inputCurves;
                delete searchArguments;
                delete vectorMetric;
                std::cout << "Error : No valid Curves were found in '" << inputFilePath << "'" << std::endl;
                return;
            }

            /* Get the vectorial representations of the just parsed Curves */
            AppliedSearch::getVectorialRepresentations(*inputCurves, vectorialInputCurves);
            vectorDimensions = vectorialInputCurves[0]->getDimensions();

            /* Optimal w calculation. Percentage argument is 0 here due to the fact the provided Input/Query files are of a minimal size */
            std::cout << "\nCalculating optimal w..." << std::endl;
            w = Method::calculateOptimalWindow(0.0, vectorialInputCurves, vectorMetric);

            VectorMethod *vectorMethod;

            /* (vectorialInputCurves.size() / x) + 1 in case (vectorialInputCurves.size() / x) == 0 */
            unsigned int threshold = (vectorialInputCurves.size() / 3) + 1;

            if (algorithm == "LSH") {

                /* (vectorialInputCurves.size() / x) + 1 in case (vectorialInputCurves.size() / x) == 0 */
                unsigned int hashTableSize = (vectorialInputCurves.size() / 8) + 1;

                vectorMethod = new VectorLSH(vectorDimensions, AppliedSearch::norm, k, w, searchArguments->getL(), hashTableSize, threshold);
            }

            else
                vectorMethod = new Hypercube(vectorDimensions, AppliedSearch::norm, k, w, searchArguments->getProbes(), searchArguments->getM());


            /* Train the corresponding data structure */
            std::cout << "\nInserting Curves in the Vector-" << algorithm << " data structure..." << std::endl;
            for (Point *point: vectorialInputCurves)
                vectorMethod->insertPoint(point);

            bool keepExecuting = true;
            int totalExecutions = 1;

            std::ofstream outputFile;

            do {

                if (totalExecutions > 1) {
                    std::pair<bool, std::string> userDecision = AppliedSearch::requestDifferentQueryFilePath();
                    keepExecuting = userDecision.first;
                    queryFilePath = userDecision.second;
                }

                if (keepExecuting) {

                    double totalQueries = 0.0;
                    double averageTrueTime = 0.0;
                    double averageApproximateTime = 0.0;
                    double maximumApproximationFactor = std::numeric_limits<double>::min();

                    outputFile.open(outputFilePath, std::ios::app);

                    std::cout << "\nParsing Query file : '" << queryFilePath << "'..." << std::endl;

                    /* Parse the provided query file. See Parser::parseCurvesFile on the inputCurves->at(0)->getComplexity() argument */
                    std::vector<Curve *> *queryCurves = Parser::parseCurvesFile(queryFilePath, curveDimensions, 1.0, inputCurves->at(0)->getComplexity());

                    std::cout << "\nApplying Vector-" << algorithm << "..." << std::endl;

                    /* Output execution parameters */
                    outputFile << "\n\n--Execution parameters--";

                    if (algorithm == "LSH") {
                        outputFile << "\n\nAlgorithm : LSH_Vector";
                        outputFile << "\nL : " + std::to_string(searchArguments->getL());
                        outputFile << "\nk : " + std::to_string(searchArguments->getK());
                        outputFile << "\nw : " + std::to_string(w);
                        outputFile << "\nthreshold : " + std::to_string(threshold);
                    }
                    else {
                        outputFile << "\n\nAlgorithm : Hypercube";
                        outputFile << "\nk : " + std::to_string(searchArguments->getK());
                        outputFile << "\nM : " + std::to_string(searchArguments->getM());
                        outputFile << "\nprobes : " + std::to_string(searchArguments->getProbes());
                        outputFile << "\nw : " + std::to_string(w);
                    }
                    outputFile.flush();

                    for (Curve *queryCurve: *queryCurves) {

                        outputFile << "\n\nQuery : " + queryCurve->getID();

                        std::tuple<Point *, long double, double> approximateNearestNeighbor = vectorMethod->getApproximateNearestNeighbor(queryCurve->getVectorialRepresentation(), vectorMetric);
                        std::tuple<Point *, long double, double> bruteForceNeighbor = vectorMethod->getExactNeighbor(queryCurve->getVectorialRepresentation(), vectorialInputCurves, vectorMetric);

                        Point *approximateNearestNeighborPoint = std::get<0>(approximateNearestNeighbor);
                        long double approximateNearestNeighborDistance = std::get<1>(approximateNearestNeighbor);
                        double approximateNearestNeighborDuration = std::get<2>(approximateNearestNeighbor);

                        long double bruteForceNeighborDistance = std::get<1>(bruteForceNeighbor);
                        double bruteForceNeighborDuration = std::get<2>(bruteForceNeighbor);

                        /* See README for more -- section Output file */
                        if (approximateNearestNeighborPoint != nullptr)
                            outputFile << "\nApproximate Nearest Neighbor : " + std::get<0>(approximateNearestNeighbor)->getCurve()->getID();
                        else
                            outputFile << "\nApproximate Nearest Neighbor : N/A";

                        outputFile << "\nTrue Nearest Neighbor : " + std::get<0>(bruteForceNeighbor)->getCurve()->getID();

                        if (approximateNearestNeighborPoint != nullptr)
                            outputFile << "\ndistanceApproximate : " + std::to_string(approximateNearestNeighborDistance);
                        else
                            outputFile << "\ndistanceApproximate : N/A";

                        outputFile << "\ndistanceTrue : " + std::to_string(bruteForceNeighborDistance);

                        if (approximateNearestNeighborPoint != nullptr)
                            outputFile << "\ntimeApproximate : " + std::to_string(approximateNearestNeighborDuration) + " ms";
                        else
                            outputFile << "\ntimeApproximate : N/A";

                        outputFile << "\ntimeTrue : " + std::to_string(bruteForceNeighborDuration) + " ms";
                        outputFile.flush();


                        if (approximateNearestNeighborPoint != nullptr) {

                            totalQueries += 1.0;

                            averageApproximateTime += approximateNearestNeighborDuration;
                            averageTrueTime += bruteForceNeighborDuration;

                            if (bruteForceNeighborDistance == approximateNearestNeighborDistance && maximumApproximationFactor < 1)
                                maximumApproximationFactor = 1;

                            else if (bruteForceNeighborDistance == 0.0 && approximateNearestNeighborDistance > 0)
                                maximumApproximationFactor = std::numeric_limits<double>::max();

                            else if (approximateNearestNeighborDistance / bruteForceNeighborDistance > maximumApproximationFactor)
                                maximumApproximationFactor = approximateNearestNeighborDistance / bruteForceNeighborDistance;
                        }

                    }

                    /* Calculate the average approximate time and average true time based on the number of valid Queries */
                    if (totalQueries > 0.0) {
                        averageApproximateTime /= totalQueries;
                        averageTrueTime /= totalQueries;
                    }

                    outputFile << "\n\ntApproximateAverage : " + std::to_string(averageApproximateTime) + " ms";
                    outputFile << "\ntApproximateTrue : " + std::to_string(averageTrueTime) + " ms";

                    /* In case no valid query Curves were found in the query file */
                    if (maximumApproximationFactor == std::numeric_limits<double>::min())
                        outputFile << "\nMAF : N/A";

                    else if (maximumApproximationFactor != std::numeric_limits<double>::max())
                        outputFile << "\nMAF : " + std::to_string(maximumApproximationFactor);

                        /* In case of division by 0 */
                    else
                        outputFile << "\nMAF : +∞";


                    for (Curve *queryCurve: *queryCurves)
                        delete queryCurve;

                    delete queryCurves;

                    totalExecutions++;

                    outputFile.close();
                }

            } while (keepExecuting);


            delete vectorMethod;
        }

        else {

            bool continuous;
            CurveMetric *curveMetric;
            unsigned int initialCurveComplexity;
            long double delta = searchArguments->getDelta();
            long double epsilon = searchArguments->getEpsilon();

            if (metric == "discrete") {
                curveDimensions = 2;
                continuous = false;
                curveMetric = new DiscreteFrechet();
            }

            else {
                curveDimensions = 1;
                continuous = true;
                curveMetric = new ContinuousFrechet();
            }

            std::cout << "\nParsing Input file : '" << inputFilePath << "'..." << std::endl;

            /* Parse the provided Input file */
            inputCurves = Parser::parseCurvesFile(inputFilePath, curveDimensions, 1.0, 0);

            /* See Parser::parseCurvesFile for more */
            if (inputCurves->empty()) {
                delete inputCurves;
                delete searchArguments;
                delete curveMetric;
                delete vectorMetric;
                std::cout << "\nError : No valid Curves were found in '" << inputFilePath << "'" << std::endl;
                return;
            }

            /* initialCurveComplexity stores the Curve-dataset pre-filtering complexity in case of LSH_Frechet_Continuous */
            initialCurveComplexity = inputCurves->at(0)->getComplexity();

            /* In case delta was not provided */
            if (delta == 0.0) {
                delta = CurveMethod::calculateOptimalDelta(*inputCurves, curveDimensions, 5);
                std::cout << "\nCalculating optimal delta..." << std::endl;
            }

            /* Get the vectorial representations of the just parsed Input Curves */
            AppliedSearch::getVectorialRepresentations(*inputCurves, vectorialInputCurves);
            vectorDimensions = vectorialInputCurves[0]->getDimensions();

            /* Optimal w calculation. Percentage argument is 0 here due to the fact the provided Input/Query files are of a minimal size */
            w = Method::calculateOptimalWindow(0.0, vectorialInputCurves, vectorMetric);
            std::cout << "\nCalculating optimal w..." << std::endl;

            /* Curve filtering */
            if (metric == "continuous") {

                std::cout << "\nFiltering Input Curves..." << std::endl;

                for (Curve *curve: *inputCurves)
                    curve->filter(epsilon);
            }


            /* (vectorialInputCurves.size() / x) + 1 in case (vectorialInputCurves.size() / x) == 0 */
            unsigned int hashTableSize = (vectorialInputCurves.size() / 8) + 1;

            /* (vectorialInputCurves.size() / x) + 1 in case (vectorialInputCurves.size() / x) == 0 */
            unsigned int threshold = (vectorialInputCurves.size() / 3) + 1;

            CurveMethod *curveMethod = new CurveLSH(vectorDimensions, AppliedSearch::norm, k, w, searchArguments->getL(), hashTableSize, threshold, curveDimensions, delta, continuous);

            std::cout << "\nInserting Curves in the Curve-LSH data structure..." << std::endl;

            /* Train the CurveLSH data structure */
            for (Curve *curve: *inputCurves)
                curveMethod->insertCurve(curve);

            bool keepExecuting = true;
            int totalExecutions = 1;

            std::ofstream outputFile;

            do {

                if (totalExecutions > 1) {
                    std::pair<bool, std::string> userDecision = AppliedSearch::requestDifferentQueryFilePath();
                    keepExecuting = userDecision.first;
                    queryFilePath = userDecision.second;
                }

                if (keepExecuting) {

                    double totalQueries = 0.0;
                    double averageTrueTime = 0.0;
                    double averageApproximateTime = 0.0;
                    double maximumApproximationFactor = std::numeric_limits<double>::min();

                    outputFile.open(outputFilePath, std::ios::app);

                    std::cout << "\nParsing Query file : '" << queryFilePath << "'..." << std::endl;

                    /* Parse the provided query file. See Parser::parseCurvesFile on the initialCurveComplexity argument */
                    std::vector<Curve *> *queryCurves = Parser::parseCurvesFile(queryFilePath, curveDimensions, 1.0, initialCurveComplexity);


                    /* Curve filtering */
                    if (metric == "continuous") {

                        std::cout << "\nFiltering Query Curves..." << std::endl;

                        for (Curve *curve: *queryCurves)
                            curve->filter(epsilon);
                    }


                    std::cout << "\nApplying Curve-LSH..." << std::endl;

                    /* Output the execution parameters */
                    outputFile << "\n\n--Execution parameters--";

                    if (metric == "discrete")
                        outputFile << "\n\nAlgorithm : LSH_Frechet_Discrete";

                    else
                        outputFile << "\n\nAlgorithm : LSH_Frechet_Continuous";


                    outputFile << "\nL : " + std::to_string(searchArguments->getL());
                    outputFile << "\nk : " + std::to_string(searchArguments->getK());
                    outputFile << "\nw : " + std::to_string(w);
                    outputFile << "\ndelta : " + std::to_string(delta);
                    outputFile << "\nthreshold : " + std::to_string(threshold);
                    if (continuous)
                        outputFile << "\nepsilon : " + std::to_string(epsilon);
                    outputFile.flush();

                    for (Curve *queryCurve: *queryCurves) {

                        outputFile << "\n\nQuery : " + queryCurve->getID();

                        std::tuple<Curve *, long double, double> approximateNearestNeighbor = curveMethod->getApproximateNearestNeighbor(queryCurve, curveMetric);
                        std::tuple<Curve *, long double, double> bruteForceNeighbor = curveMethod->getExactNeighbor(queryCurve, *inputCurves, curveMetric);

                        Curve *approximateNearestNeighborCurve = std::get<0>(approximateNearestNeighbor);
                        double approximateNearestNeighborDuration = std::get<2>(approximateNearestNeighbor);
                        long double approximateNearestNeighborDistance = std::get<1>(approximateNearestNeighbor);

                        long double bruteForceNeighborDistance = std::get<1>(bruteForceNeighbor);
                        double bruteForceNeighborDuration = std::get<2>(bruteForceNeighbor);

                        /* See README for more -- section Output file */
                        if (approximateNearestNeighborCurve != nullptr)
                            outputFile << "\nApproximate Nearest Neighbor : " + std::get<0>(approximateNearestNeighbor)->getID();
                        else
                            outputFile << "\nApproximate Nearest Neighbor : N/A";

                        outputFile << "\nTrue Nearest Neighbor : " + std::get<0>(bruteForceNeighbor)->getID();

                        if (approximateNearestNeighborCurve != nullptr)
                            outputFile << "\ndistanceApproximate : " + std::to_string(approximateNearestNeighborDistance);
                        else
                            outputFile << "\ndistanceApproximate : N/A";

                        outputFile << "\ndistanceTrue : " + std::to_string(bruteForceNeighborDistance);

                        if (approximateNearestNeighborCurve != nullptr)
                            outputFile << "\ntimeApproximate : " + std::to_string(approximateNearestNeighborDuration) + " ms";
                        else
                            outputFile << "\ntimeApproximate : N/A";

                        outputFile << "\ntimeTrue : " + std::to_string(bruteForceNeighborDuration) + " ms";
                        outputFile.flush();

                        if (approximateNearestNeighborCurve != nullptr) {

                            totalQueries += 1.0;

                            averageApproximateTime += approximateNearestNeighborDuration;
                            averageTrueTime += bruteForceNeighborDuration;

                            if (bruteForceNeighborDistance == approximateNearestNeighborDistance && maximumApproximationFactor < 1)
                                maximumApproximationFactor = 1;

                            else if (bruteForceNeighborDistance == 0.0 && approximateNearestNeighborDistance > 0)
                                maximumApproximationFactor = std::numeric_limits<double>::max();

                            else if (approximateNearestNeighborDistance / bruteForceNeighborDistance > maximumApproximationFactor)
                                maximumApproximationFactor = approximateNearestNeighborDistance / bruteForceNeighborDistance;

                        }

                    }

                    /* Calculate the average approximate time and average true time based on the number of valid Queries */
                    if (totalQueries > 0.0) {
                        averageApproximateTime /= totalQueries;
                        averageTrueTime /= totalQueries;
                    }

                    outputFile << "\n\ntApproximateAverage : " + std::to_string(averageApproximateTime) + " ms";
                    outputFile << "\ntApproximateTrue : " + std::to_string(averageTrueTime) + " ms";

                    /* In case no valid Query Curves were found in the Query file */
                    if (maximumApproximationFactor == std::numeric_limits<double>::min())
                        outputFile << "\nMAF : N/A";

                    else if (maximumApproximationFactor != std::numeric_limits<double>::max())
                        outputFile << "\nMAF : " + std::to_string(maximumApproximationFactor);

                        /* In case of a division by 0 */
                    else
                        outputFile << "\nMAF : +∞";

                    for (Curve *queryCurve: *queryCurves)
                        delete queryCurve;

                    delete queryCurves;

                    totalExecutions++;

                    outputFile.close();
                }

            } while (keepExecuting);

            delete curveMetric;
            delete curveMethod;
        }


        for (Curve *curve: *inputCurves)
            delete curve;
        delete inputCurves;
        delete vectorMetric;
        delete searchArguments;

    }

}

/* Utility function to request the file path of an existing, not empty and in Linux format file */
std::string AppliedSearch::requestInputFilePath() {

    std::string inputFilePath;

    do {
        std::cout << "\nProvide the path of the Input File : ";
        std::getline(std::cin, inputFilePath);
        std::cout << std::endl;
    } while (!File::canOpenFile(inputFilePath) || File::isFileEmpty(inputFilePath) || File::isInWindowsFormat(inputFilePath));

    return inputFilePath;
}

/* Utility function to request the file path of an existing file */
std::string AppliedSearch::requestOutputFilePath() {

    std::string outputFilePath;

    if (outputFilePath.empty())
        do {
            std::cout << "\nProvide the path of the Output File : ";
            std::getline(std::cin, outputFilePath);
            std::cout << std::endl;
        } while (!File::canOpenFile(outputFilePath));


    return outputFilePath;
}

/* Utility function to request the file path of an existing, not empty and in Linux format file */
std::string AppliedSearch::requestQueryFilePath() {

    std::string queryFilePath;

    do {
        std::cout << "\nProvide the path of the Query File : ";
        std::getline(std::cin, queryFilePath);
        std::cout << std::endl;
    } while (!File::canOpenFile(queryFilePath) || File::isFileEmpty(queryFilePath) || File::isInWindowsFormat(queryFilePath));

    return queryFilePath;
}

/* Utility function to request the appropriate algorithm */
std::string AppliedSearch::requestAlgorithm(SearchArguments *searchArguments) {

    std::string algorithm;

    do {
        std::cout << "\nProvide the algorithm you want to apply : ";
        std::getline(std::cin, algorithm);
        std::cout << std::endl;
    } while (algorithm != "LSH" && algorithm != "Hypercube" && algorithm != "Frechet");

    if (algorithm == "LSH") {
        searchArguments->setL(5);
        searchArguments->setK(4);
    }

    else if (algorithm == "Hypercube") {
        searchArguments->setK(14);
        searchArguments->setM(10);
        searchArguments->setProbes(2);
    }

    else
        searchArguments->setK(4);


    return algorithm;
}

/* Utility function to request the appropriate Frechet metric */
std::string AppliedSearch::requestFrechetMetric(SearchArguments *searchArguments) {

    std::string metric;

    do {
        std::cout << "\nProvide the desired Frechet metric : ";
        std::getline(std::cin, metric);
        std::cout << std::endl;
    } while (metric != "discrete" && metric != "continuous");

    if (metric == "discrete")
        searchArguments->setL(5);

    else
        searchArguments->setL(1);

    return metric;
}

/* Utility function to query user for an additional execution */
std::pair<bool, std::string> AppliedSearch::requestDifferentQueryFilePath() {

    std::string userInput;

    do {
        std::cout << "\nDo you wish to continue using another Query File? (Y/N) : ";
        std::getline(std::cin, userInput);
        std::cout << std::endl;
    } while (userInput != "y" && userInput != "n" && userInput != "Y" && userInput != "N");


    if (userInput == "n" || userInput == "N")
        return std::make_pair(false, "");


    else
        do {
            std::cout << "\nProvide the path of the Query File : ";
            std::getline(std::cin, userInput);
            std::cout << std::endl;
        } while (!File::canOpenFile(userInput) || File::isFileEmpty(userInput) || File::isInWindowsFormat(userInput));


    return std::make_pair(true, userInput);

}


#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "ClusteringParameters.h"
#include "../../Utilities/String/String.h"

ClusteringParameters::ClusteringParameters(unsigned int kMeans, unsigned int L, unsigned int LSHK, unsigned int M, unsigned int probes, unsigned int HypercubeK) {

    this->kMeans = kMeans;
    this->L = L;
    this->LSHK = LSHK;
    this->M = M;
    this->probes = probes;
    this->HypercubeK = HypercubeK;
}

unsigned int ClusteringParameters::getKMeans() const {
    return this->kMeans;
}

unsigned int ClusteringParameters::getL() const {
    return this->L;
}

unsigned int ClusteringParameters::getLSHK() const {
    return this->LSHK;
}

unsigned int ClusteringParameters::getM() const {
    return this->M;
}

unsigned int ClusteringParameters::getProbes() const {
    return this->probes;
}

unsigned int ClusteringParameters::getHypercubeK() const {
    return this->HypercubeK;
}

/* Utility function to parse the cluster.conf file */
ClusteringParameters *ClusteringParameters::parseConfigurationFile(const std::string &filePath) {

    unsigned int kMeans;
    unsigned int L = 3;
    unsigned int LSHK = 4;
    unsigned int M = 10;
    unsigned int probes = 2;
    unsigned int HypercubeK = 3;

    std::ifstream file;
    file.open(filePath);

    std::string fileLine;
    std::vector<std::string> fileLines;

    while (std::getline(file, fileLine))
        fileLines.push_back(fileLine);

    file.close();


    /* cluster.conf file should have a specific number of lines */
    if (fileLines.size() < 1 || fileLines.size() > 6) {
        std::cout << "Error : Invalid number of lines in : " << filePath << std::endl;
        return nullptr;
    }


    /* Create a dictionary so that we know which lines were provided */
    std::unordered_map<std::string, bool> linesProvided({{"number_of_clusters:",              false},
                                                         {"number_of_vector_hash_tables:",    false},
                                                         {"number_of_vector_hash_functions:", false},
                                                         {"max_number_M_hypercube:",          false},
                                                         {"number_of_hypercube_dimensions:",  false},
                                                         {"number_of_probes:",                false}});

    for (std::string line: fileLines) {

        std::vector<std::string> *lineTokens = String::tokenizeString(line, ' ');

        /* Each line is considered to be valid as long as it consists of 2 tokens */
        if (lineTokens->size() != 2) {
            delete lineTokens;
            std::cout << "Error : Invalid number of tokens in line : " << line << std::endl;
            return nullptr;
        }

        std::string lineKey = lineTokens->at(0);
        std::string lineValue = lineTokens->at(1);

        /* Each line can only be provided once */
        if (linesProvided[lineKey]) {
            delete lineTokens;
            std::cout << "Error : '" << lineKey << "' was already provided" << std::endl;
            return nullptr;
        }

        if (lineKey == "number_of_clusters:") {

            /* Current line is invalid due to the fact that its value is not a positive integer */
            if (!String::isWholeNumber(lineValue) || std::stoi(lineValue) <= 0) {
                delete lineTokens;
                std::cout << "Error : 'number_of_clusters' should be a positive integer" << std::endl;
                return nullptr;
            }

            linesProvided[lineKey] = true;
            kMeans = std::stoi(lineValue);
        }

        else if (lineKey == "number_of_vector_hash_tables:") {

            /* Current line is invalid due to the fact that its value is not a positive integer */
            if (!String::isWholeNumber(lineValue) || std::stoi(lineValue) <= 0) {
                delete lineTokens;
                std::cout << "Error : 'number_of_vector_hash_tables' should be a positive integer" << std::endl;
                return nullptr;
            }

            linesProvided[lineKey] = true;
            L = std::stoi(lineValue);
        }

        else if (lineKey == "number_of_vector_hash_functions:") {

            /* Current line is invalid due to the fact that its value is not a positive integer */
            if (!String::isWholeNumber(lineValue) || std::stoi(lineValue) <= 0) {
                delete lineTokens;
                std::cout << "Error : 'number_of_vector_hash_functions' should be a positive integer" << std::endl;
                return nullptr;
            }

            linesProvided[lineKey] = true;
            LSHK = std::stoi(lineValue);
        }

        else if (lineKey == "max_number_M_hypercube:") {

            /* Current line is invalid due to the fact that its value is not a positive integer */
            if (!String::isWholeNumber(lineValue) || std::stoi(lineValue) <= 0) {
                delete lineTokens;
                std::cout << "Error : 'max_number_M_hypercube' should be a positive integer" << std::endl;
                return nullptr;
            }

            linesProvided[lineKey] = true;
            M = std::stoi(lineValue);
        }

        else if (lineKey == "number_of_hypercube_dimensions:") {

            /* Current line is invalid due to the fact that its value is not a positive integer */
            if (!String::isWholeNumber(lineValue) || std::stoi(lineValue) <= 0) {
                delete lineTokens;
                std::cout << "Error : 'number_of_hypercube_dimensions' should be a positive integer" << std::endl;
                return nullptr;
            }

            linesProvided[lineKey] = true;
            HypercubeK = std::stoi(lineValue);
        }

        else if (lineKey == "number_of_probes:") {

            /* Current line is invalid due to the fact that its value is not a positive integer */
            if (!String::isWholeNumber(lineValue) || std::stoi(lineValue) <= 0) {
                delete lineTokens;
                std::cout << "Error : 'number_of_probes' should be a positive integer" << std::endl;
                return nullptr;
            }

            linesProvided[lineKey] = true;
            probes = std::stoi(lineValue);
        }

        /* Current line is invalid */
        else {
            delete lineTokens;
            std::cout << "Error : The following line is invalid : " << line << std::endl;
            return nullptr;
        }

        delete lineTokens;
    }

    /* In case the 'number_of_clusters' line was not provided then the parsing of the cluster.conf file fails */
    if (!linesProvided["number_of_clusters:"]) {
        std::cout << "Error : 'number_of_clusters' was not provided" << std::endl;
        return nullptr;
    }

    return new ClusteringParameters(kMeans, L, LSHK, M, probes, HypercubeK);
}




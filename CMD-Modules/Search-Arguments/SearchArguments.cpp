#include <iostream>
#include "SearchArguments.h"
#include "../../Utilities/File/File.h"
#include "../../Utilities/String/String.h"


const std::string &SearchArguments::getInputFilePath() const {
    return this->inputFilePath;
}

const std::string &SearchArguments::getOutputFilePath() const {
    return this->outputFilePath;
}

const std::string &SearchArguments::getQueryFilePath() const {
    return this->queryFilePath;
}

unsigned int SearchArguments::getK() const {
    return this->k;
}

unsigned int SearchArguments::getL() const {
    return this->L;
}

unsigned int SearchArguments::getM() const {
    return this->M;
}

unsigned int SearchArguments::getProbes() const {
    return this->probes;
}

long double SearchArguments::getDelta() const {
    return this->delta;
}

const std::string &SearchArguments::getAlgorithm() const {
    return this->algorithm;
}

const std::string &SearchArguments::getMetric() const {
    return this->metric;
}

SearchArguments *SearchArguments::parseCMDArguments(int argc, char **argv) {

    if (argc == 1)
        return new SearchArguments();


    if ((argc - 1) % 2 != 0 || argc > 23) {
        std::cout << "Error : Invalid number of arguments was provided" << std::endl;
        return nullptr;
    }


    SearchArguments *searchArguments = new SearchArguments();

    bool parseSuccessful = true;
    /* Create a dictionary to mark already provided arguments */
    std::unordered_map<std::string, bool> argumentsProvided({{"-i",         false},
                                                             {"-q",         false},
                                                             {"-k",         false},
                                                             {"-L",         false},
                                                             {"-M",         false},
                                                             {"-probes",    false},
                                                             {"-o",         false},
                                                             {"-algorithm", false},
                                                             {"-metric",    false},
                                                             {"-delta",     false},
                                                             {"-e",         false}});


    SearchArguments::parseCMDArguments(argv, argc, argumentsProvided, 1, *searchArguments, parseSuccessful);

    /* Numeric parameters can only be provided alongside the '-algorithm' parameter */
    if (parseSuccessful && !argumentsProvided["-algorithm"] && (argumentsProvided["-e"] || argumentsProvided["-L"] || argumentsProvided["-probes"] || argumentsProvided["-k"] || argumentsProvided["-M"] || argumentsProvided["-delta"])) {
        std::cout << "Error : Numeric parameters can only be provided alongside the '-algorithm' parameter" << std::endl;
        parseSuccessful = false;
    }

    /* The '-metric' parameter can only be provided alongside the '-algorithm Frechet' parameter */
    if (parseSuccessful && argumentsProvided["-metric"] && searchArguments->algorithm != "Frechet") {
        std::cout << "Error : The '-metric' parameter can only be provided alongside the '-algorithm Frechet' parameter" << std::endl;
        parseSuccessful = false;
    }

    /* The '-delta' parameter can only be provided alongside the '-algorithm Frechet' parameter */
    if (parseSuccessful && argumentsProvided["-delta"] && searchArguments->algorithm != "Frechet") {
        std::cout << "Error : The '-delta' parameter can only be provided alongside the '-algorithm Frechet' parameter" << std::endl;
        parseSuccessful = false;
    }

    /* The '-e' parameter can only be provided alongside the '-algorithm Frechet' and '-metric continuous' parameters combination */
    if (parseSuccessful && argumentsProvided["-e"] && searchArguments->algorithm == "Frechet" && searchArguments->metric != "continuous") {
        std::cout << "Error : The '-e' parameter can only be provided alongside the '-algorithm Frechet' and '-metric continuous' parameters combination" << std::endl;
        parseSuccessful = false;
    }

    else if (parseSuccessful && argumentsProvided["-e"] && searchArguments->algorithm != "Frechet") {
        std::cout << "Error : The '-e' parameter can only be provided alongside the '-algorithm Frechet' and '-metric continuous' parameters combination" << std::endl;
        parseSuccessful = false;
    }


    if (parseSuccessful && !searchArguments->algorithm.empty()) {

        if (searchArguments->algorithm == "LSH" || searchArguments->algorithm == "Frechet") {

            /* Parameters '-M' and/or '-probes' cannot be provided alongside the '-algorithm LSH' or the '-algorithm Frechet' parameter */
            if (searchArguments->M != 0 || searchArguments->probes != 0) {
                std::cout << "Error : Parameters '-M' and/or '-probes' cannot be provided alongside the '-algorithm LSH' or the '-algorithm Frechet' parameter" << std::endl;
                parseSuccessful = false;
            }

            /* Parameter '-L' can only be equal to 1 alongside the '-algorithm Frechet' and '-metric continuous' parameters combination */
            if (parseSuccessful && searchArguments->algorithm == "Frechet" && searchArguments->metric == "continuous" && searchArguments->L > 1) {
                std::cout << "Error : Parameter '-L' can only be equal to 1 alongside the '-algorithm Frechet' and '-metric continuous' parameters combination" << std::endl;
                parseSuccessful = false;
            }

            /* In case the numeric parameters for LSH or Frechet were not provided assign the default values */
            if (parseSuccessful && searchArguments->k == 0)
                searchArguments->k = 4;

            if (parseSuccessful && searchArguments->L == 0 && searchArguments->algorithm == "LSH")
                searchArguments->L = 5;

            if (parseSuccessful && searchArguments->L == 0 && searchArguments->algorithm == "Frechet" && searchArguments->metric == "continuous")
                searchArguments->L = 1;

            else if (parseSuccessful && searchArguments->L == 0 && searchArguments->algorithm == "Frechet" && searchArguments->metric == "discrete")
                searchArguments->L = 5;
        }

        else {

            /* Parameter '-L' cannot be provided alongside the '-algorithm Hypercube' parameter */
            if (searchArguments->L != 0) {
                std::cout << "Error : Parameter '-L' cannot be provided alongside the '-algorithm Hypercube' parameter" << std::endl;
                parseSuccessful = false;
            }

            /* In case the numeric parameters for Hypercube were not provided assign the default values */
            if (parseSuccessful && searchArguments->k == 0)
                searchArguments->k = 14;

            if (parseSuccessful && searchArguments->M == 0)
                searchArguments->M = 10;

            if (parseSuccessful && searchArguments->probes == 0)
                searchArguments->probes = 2;

        }

    }


    if (!parseSuccessful) {
        delete searchArguments;
        return nullptr;
    }

    return searchArguments;

}

void SearchArguments::parseCMDArguments(char **argv, int argc, std::unordered_map<std::string, bool> &argumentsProvided, int i, SearchArguments &searchArguments, bool &parseSuccessful) {

    if (i >= argc)
        return;

    std::string parameter(argv[i]);
    std::string value(argv[i + 1]);

    /* Parameter say X = '-X' was already provided so the parsing of the arguments fails */
    if (argumentsProvided[parameter]) {
        std::cout << "Error : Parameter '" << parameter << "' was already provided" << std::endl;
        parseSuccessful = false;
        return;
    }

    if (parameter == "-i") {

        /* Current parameter is invalid due to the fact that the file associated with the file path provided could not be opened */
        if (!File::canOpenFile(value)) {
            std::cout << "Error : Could not open file " << value << std::endl;
            parseSuccessful = false;
            return;
        }

        /* Current parameter is invalid due to the fact that the file associated with the file path provided is empty */
        if (File::isFileEmpty(value)) {
            std::cout << "Error : File " << value << " is empty" << std::endl;
            parseSuccessful = false;
            return;
        }

        /* Current parameter is invalid due to the fact that the file associated with the file path provided is in Windows format */
        if (File::isInWindowsFormat(value)) {
            std::cout << "Error : File " << value << " is in Windows format meaning its new line character set is '\\r\\n' " << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.inputFilePath = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }
    }

    else if (parameter == "-q") {

        /* Current parameter is invalid due to the fact that the file associated with the file path provided could not be opened */
        if (!File::canOpenFile(value)) {
            std::cout << "Error : Could not open file " << value << std::endl;
            parseSuccessful = false;
            return;
        }

        /* Current parameter is invalid due to the fact that the file associated with the file path provided is empty */
        if (File::isFileEmpty(value)) {
            std::cout << "Error : File " << value << " is empty" << std::endl;
            parseSuccessful = false;
            return;
        }

        /* Current parameter is invalid due to the fact that the file associated with the file path provided is in Windows format */
        if (File::isInWindowsFormat(value)) {
            std::cout << "Error : File " << value << " is in Windows format meaning its new line character set is '\\r\\n' " << std::endl;
            parseSuccessful = false;
            return;
        }


        else {
            argumentsProvided[parameter] = true;
            searchArguments.queryFilePath = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }

    else if (parameter == "-k") {

        /* Current parameter is invalid due to the fact that its value is not a positive integer */
        if (!String::isWholeNumber(value) || std::stoi(value) <= 0) {
            std::cout << "Error : Parameter '" << parameter << "' should be a positive integer" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.k = std::stoi(value);
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }
    }

    else if (parameter == "-L") {

        /* Current parameter is invalid due to the fact that its value is not a positive integer */
        if (!String::isWholeNumber(value) || std::stoi(value) <= 0) {
            std::cout << "Error : Parameter '" << parameter << "' should be a positive integer" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.L = std::stoi(value);
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }

    else if (parameter == "-M") {

        /* Current parameter is invalid due to the fact that its value is not a positive integer */
        if (!String::isWholeNumber(value) || std::stoi(value) <= 0) {
            std::cout << "Error : Parameter '" << parameter << "' should be a positive integer" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.M = std::stoi(value);
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }

    else if (parameter == "-probes") {

        /* Current parameter is invalid due to the fact that its value is not a positive integer */
        if (!String::isWholeNumber(value) || std::stoi(value) <= 0) {
            std::cout << "Error : Parameter '" << parameter << "' should be a positive integer" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.probes = std::stoi(value);
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }

    else if (parameter == "-delta") {

        /* Current parameter is invalid due to the fact that its value is not a positive real number */
        if (!String::isFloatingPointNumber(value) || std::stold(value) <= 0) {
            std::cout << "Error : Parameter '" << parameter << "' should be a positive real number" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.delta = std::stold(value);
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }

    else if (parameter == "-e") {

        /* Current parameter is invalid due to the fact that its value is not a positive real number */
        if (!String::isFloatingPointNumber(value) || std::stold(value) < 0) {
            std::cout << "Error : Parameter '" << parameter << "' should be a positive real number" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.epsilon = std::stold(value);
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }


    else if (parameter == "-o") {

        /* Current parameter is invalid due to the fact that the file associated with the file path provided could not be opened */
        if (!File::canOpenFile(value)) {
            std::cout << "Error : Could not open file " << value << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.outputFilePath = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }

    else if (parameter == "-algorithm") {

        /* Current parameter is invalid due to the fact that its value is not LSH, Hypercube or Frechet */
        if (value != "LSH" && value != "Hypercube" && value != "Frechet") {
            std::cout << "Error : Parameter '" << parameter << "' should be either 'LSH', 'Hypercube' or 'Frechet'" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.algorithm = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }

    else if (parameter == "-metric") {

        /* Current parameter is invalid due to the fact that its value is not discrete or continuous */
        if (value != "discrete" && value != "continuous") {
            std::cout << "Error : Parameter '" << parameter << "' should be either 'discrete' or 'continuous'" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            searchArguments.metric = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, searchArguments, parseSuccessful);
        }

    }


        /* Current parameter is invalid */
    else {
        std::cout << "Error : Parameter '" << parameter << "' is invalid" << std::endl;
        parseSuccessful = false;
        return;
    }


}

void SearchArguments::setK(unsigned int k) {
    this->k = k;
}

void SearchArguments::setL(unsigned int L) {
    this->L = L;
}

void SearchArguments::setM(unsigned int M) {
    this->M = M;
}

void SearchArguments::setProbes(unsigned int probes) {
    this->probes = probes;
}

long double SearchArguments::getEpsilon() const {
    return this->epsilon;
}

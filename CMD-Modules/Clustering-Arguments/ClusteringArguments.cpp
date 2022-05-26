#include <iostream>
#include "ClusteringArguments.h"
#include "../../Utilities/File/File.h"
#include "../../Utilities/String/String.h"

const std::string &ClusteringArguments::getInputFilePath() const {
    return this->inputFilePath;
}

const std::string &ClusteringArguments::getConfigurationFilePath() const {
    return this->configurationFilePath;
}

const std::string &ClusteringArguments::getOutputFilePath() const {
    return this->outputFilePath;
}

const std::string &ClusteringArguments::getUpdateMethod() const {
    return this->updateMethod;
}

const std::string &ClusteringArguments::getAssignmentMethod() const {
    return this->assignmentMethod;
}

bool ClusteringArguments::shouldCalculateSilhouette() const {
    return this->calculateSilhouette;
}

bool ClusteringArguments::isComplete() const {
    return this->complete;
}

ClusteringArguments *ClusteringArguments::parseCMDArguments(int argc, char **argv) {


    if ((argc - 1) % 2 != 0 || argc > 15 || argc < 9) {
        std::cout << "Error : Invalid number of arguments was provided" << std::endl;
        return nullptr;
    }


    ClusteringArguments *clusteringArguments = new ClusteringArguments();


    bool parseSuccessful = true;
    /* Create a dictionary to mark already provided arguments */
    std::unordered_map<std::string, bool> argumentsProvided({{"-i",          false},
                                                             {"-c",          false},
                                                             {"-o",          false},
                                                             {"-update",     false},
                                                             {"-assignment", false},
                                                             {"-complete",   false},
                                                             {"-silhouette", false}});


    ClusteringArguments::parseCMDArguments(argv, argc, argumentsProvided, 1, *clusteringArguments, parseSuccessful);

    /* Mandatory parameter '-i' was not provided so the parsing of the arguments fails */
    if (parseSuccessful && !argumentsProvided["-i"]) {
        std::cout << "Error : Parameter '-i' was not provided" << std::endl;
        parseSuccessful = false;
    }

    /* Mandatory parameter '-c' was not provided so the parsing of the arguments fails */
    if (parseSuccessful && !argumentsProvided["-c"]) {
        std::cout << "Error : Parameter '-c' was not provided" << std::endl;
        parseSuccessful = false;
    }

    /* Mandatory parameter '-assignment' was not provided so the parsing of the arguments fails */
    if (parseSuccessful && !argumentsProvided["-assignment"]) {
        std::cout << "Error : Parameter '-assignment' was not provided" << std::endl;
        parseSuccessful = false;
    }

    /* Mandatory parameter '-update' was not provided so the parsing of the arguments fails */
    if (parseSuccessful && !argumentsProvided["-update"]) {
        std::cout << "Error : Parameter '-update' was not provided" << std::endl;
        parseSuccessful = false;
    }

    /* Parameter '-assignment LSH' can only be provided alongside the '-update Mean-Vector' parameter */
    if (parseSuccessful && clusteringArguments->assignmentMethod == "LSH" && clusteringArguments->updateMethod != "Mean-Vector") {
        std::cout << "Error : Parameter '-assignment LSH' can only be provided alongside the '-update Mean-Vector' parameter" << std::endl;
        parseSuccessful = false;
    }

    /* Parameter '-assignment Hypercube' can only be provided alongside the '-update Mean-Vector' parameter */
    if (parseSuccessful && clusteringArguments->assignmentMethod == "Hypercube" && clusteringArguments->updateMethod != "Mean-Vector") {
        std::cout << "Error : Parameter '-assignment Hypercube' can only be provided alongside '-update Mean-Vector parameter'" << std::endl;
        parseSuccessful = false;
    }

    /*  Parameter '-assignment LSH_Frechet' can only be provided alongside '-update Mean-Frechet' parameter */
    if (parseSuccessful && clusteringArguments->assignmentMethod == "LSH_Frechet" && clusteringArguments->updateMethod != "Mean-Frechet") {
        std::cout << "Error : Parameter '-assignment LSH_Frechet' can only be provided alongside '-update Mean-Frechet' parameter" << std::endl;
        parseSuccessful = false;
    }


    if (!parseSuccessful) {
        delete clusteringArguments;
        return nullptr;
    }

    return clusteringArguments;
}

void ClusteringArguments::parseCMDArguments(char **argv, int argc, std::unordered_map<std::string, bool> &argumentsProvided, int i, ClusteringArguments &clusteringArguments, bool &parseSuccessful) {

    if (i >= argc)
        return;

    std::string parameter(argv[i]);
    std::string value(argv[i + 1]);

    /* Parameter say X = '-X' was already provided so the parsing of the arguments fails */
    if (argumentsProvided[parameter])
    {
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
            clusteringArguments.inputFilePath = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, clusteringArguments, parseSuccessful);
        }
    }

    else if (parameter == "-c") {

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
            clusteringArguments.configurationFilePath = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, clusteringArguments, parseSuccessful);
        }

    }

    else if (parameter == "-complete") {

        /* Current parameter is invalid due to the fact that its value is not a boolean value */
        if (!String::isBoolean(value)) {
            std::cout << "Error : Parameter '" << parameter << "' should be either 'true or 'false'" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            if (value == "true")
                clusteringArguments.complete = true;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, clusteringArguments, parseSuccessful);
        }

    }

    else if (parameter == "-silhouette") {

        /* Current parameter is invalid due to the fact that its value is not a boolean value */
        if (!String::isBoolean(value)) {
            std::cout << "Error : Parameter '" << parameter << "' should be either 'true or 'false'" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            if (value == "true")
                clusteringArguments.calculateSilhouette = true;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, clusteringArguments, parseSuccessful);
        }

    }

    else if (parameter == "-update") {

        /* Current parameter is invalid due to the fact that its value is not Mean-Frechet or Mean-Vector */
        if (value != "Mean-Frechet" && value != "Mean-Vector") {
            std::cout << "Error : Parameter '" << parameter << "' should be either 'Mean-Frechet' or 'Mean-Vector'" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            clusteringArguments.updateMethod = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, clusteringArguments, parseSuccessful);
        }

    }


    else if (parameter == "-assignment") {

        /* Current parameter is invalid due to the fact that its value is not Classic, LSH, Hypercube or LSH_Frechet */
        if (value != "Classic" && value != "LSH" && value != "Hypercube" && value != "LSH_Frechet") {
            std::cout << "Error : Parameter '" << parameter << "' should be either 'Classic', 'LSH', 'Hypercube' or 'LSH_Frechet'" << std::endl;
            parseSuccessful = false;
            return;
        }

        else {
            argumentsProvided[parameter] = true;
            clusteringArguments.assignmentMethod = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, clusteringArguments, parseSuccessful);
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
            clusteringArguments.outputFilePath = value;
            parseCMDArguments(argv, argc, argumentsProvided, i + 2, clusteringArguments, parseSuccessful);
        }

    }


    /* Current parameter is invalid */
    else {
        std::cout << "Error : Parameter '" << parameter << "' is invalid" << std::endl;
        parseSuccessful = false;
        return;
    }

}

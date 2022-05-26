#ifndef CLUSTERINGARGUMENTS_H
#define CLUSTERINGARGUMENTS_H


#include <string>
#include <unordered_map>

class ClusteringArguments {

private:
    std::string inputFilePath = "";
    std::string configurationFilePath = "";
    std::string outputFilePath = "";
    std::string updateMethod = "";
    std::string assignmentMethod = "";
    bool calculateSilhouette = false;
    bool complete = false;

    static void parseCMDArguments(char **, int, std::unordered_map<std::string, bool> &, int, ClusteringArguments &, bool &);

public:
    static ClusteringArguments *parseCMDArguments(int, char **);

    const std::string &getInputFilePath() const;
    const std::string &getConfigurationFilePath() const;
    const std::string &getOutputFilePath() const;
    const std::string &getUpdateMethod() const;
    const std::string &getAssignmentMethod() const;
    bool shouldCalculateSilhouette() const;
    bool isComplete() const;


};


#endif

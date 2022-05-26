#ifndef SEARCHARGUMENTS_H
#define SEARCHARGUMENTS_H


#include <string>
#include <unordered_map>

class SearchArguments {

private:
    std::string inputFilePath = "";
    std::string outputFilePath = "";
    std::string queryFilePath = "";
    unsigned int k = 0;
    unsigned int L = 0;
    unsigned int M = 0;
    unsigned int probes = 0;
    long double delta = 0.0;
    long double epsilon = 1.0;
    std::string algorithm = "";
    std::string metric = "";

    static void parseCMDArguments(char **, int, std::unordered_map<std::string, bool> &, int, SearchArguments &, bool &);

public:
    static SearchArguments *parseCMDArguments(int, char **);


    const std::string &getInputFilePath() const;
    const std::string &getOutputFilePath() const;
    const std::string &getQueryFilePath() const;
    unsigned int getK() const;
    unsigned int getL() const;
    unsigned int getM() const;
    unsigned int getProbes() const;
    long double getDelta() const;
    long double getEpsilon() const;
    const std::string &getAlgorithm() const;
    const std::string &getMetric() const;


    void setK(unsigned int);
    void setL(unsigned int);
    void setM(unsigned int);
    void setProbes(unsigned int);


};


#endif

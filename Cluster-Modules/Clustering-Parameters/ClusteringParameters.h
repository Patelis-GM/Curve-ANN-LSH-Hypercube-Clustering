#ifndef CLUSTERINGPARAMETERS_H
#define CLUSTERINGPARAMETERS_H


#include <string>

class ClusteringParameters {

private:
    unsigned int kMeans;
    unsigned int L;
    unsigned int LSHK;
    unsigned int M;
    unsigned int probes;
    unsigned int HypercubeK;

public:
    ClusteringParameters(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    static ClusteringParameters *parseConfigurationFile(const std::string &);

    unsigned int getKMeans() const;
    unsigned int getL() const;
    unsigned int getLSHK() const;
    unsigned int getM() const;
    unsigned int getProbes() const;
    unsigned int getHypercubeK() const;

};


#endif

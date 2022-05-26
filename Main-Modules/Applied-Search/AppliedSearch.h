#ifndef APPLIEDSEARCH_H
#define APPLIEDSEARCH_H

#include <string>
#include "../../Curve-Modules/Curve/Curve.h"
#include "../../CMD-Modules/Search-Arguments/SearchArguments.h"

class AppliedSearch {

private:

    static const long double norm;
    static std::string requestInputFilePath();
    static std::string requestAlgorithm(SearchArguments *);
    static std::string requestFrechetMetric(SearchArguments *searchArguments);
    static std::string requestOutputFilePath();
    static std::string requestQueryFilePath();
    static std::pair<bool, std::string> requestDifferentQueryFilePath();
    static void getVectorialRepresentations(const std::vector<Curve *> &, std::vector<Point *> &);

    public:


    static void appliedSearch(int, char **);

};


#endif
#ifndef PARSER_H
#define PARSER_H

#include "../../Curve-Modules/Curve/Curve.h"

class Parser {

public:
    static std::vector<Curve *> *parseCurvesFile(const std::string &, unsigned int, long double, unsigned int);
};


#endif

#ifndef STRING_H
#define STRING_H


#include <string>
#include <vector>

class String {

public:
    const static unsigned int INVALID_PARAMETERS;
    static std::vector<std::string> *tokenizeString(const std::string &, char);
    static bool isDigit(char);
    static bool isBoolean(const std::string &);
    static bool isWholeNumber(const std::string &);
    static bool isFloatingPointNumber(const std::string&);
    static unsigned int hammingDistance(const std::string &, const std::string &);
};


#endif

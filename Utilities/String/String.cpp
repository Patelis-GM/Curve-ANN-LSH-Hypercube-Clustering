#include <sstream>
#include "String.h"
#include <limits>

const unsigned int String::INVALID_PARAMETERS = std::numeric_limits<unsigned int>::max();


/* Utility function to tokenize a string given a delimiter */
std::vector<std::string> *String::tokenizeString(const std::string &stringToTokenize, char delimiter) {

    std::vector<std::string> *stringTokens = new std::vector<std::string>;

    std::stringstream stringStream(stringToTokenize);

    std::string stringToken;

    while (getline(stringStream, stringToken, delimiter))
        stringTokens->push_back(stringToken);

    return stringTokens;
}

/* Utility function to decide whether a character is a number or not */
bool String::isDigit(char character) {
    return character >= '0' && character <= '9';
}

/* Utility function to decide whether a std::string is a boolean value or not */
bool String::isBoolean(const std::string &string) {

    if (string == "true" || string == "false")
        return true;

    return false;
}

/* Utility function to calculate the Hamming Distance between 2 strings */
unsigned int String::hammingDistance(const std::string &stringOne, const std::string &stringTwo) {

    if (stringOne.size() != stringTwo.size())
        return String::INVALID_PARAMETERS;

   unsigned int hammingDistance = 0;

    for (unsigned int i = 0; i < stringOne.length(); ++i)
        if (stringOne[i] != stringTwo[i])
            hammingDistance++;

    return hammingDistance;
}

/* Utility function to decide whether a std::string is a floating point number or not */
bool String::isFloatingPointNumber(const std::string &string) {

    if (string.empty() || string[string.size() - 1] == '.')
        return false;

    if (string[0] != '+' && string[0] != '-' && !String::isDigit(string[0]))
        return false;

    unsigned int totalDecimalPoints = 0;

    for (unsigned int i = 1; i < string.size(); ++i) {

        if (!String::isDigit(string[i]) && string[i] != '.')
            return false;

        if (string[i] == '.')
            totalDecimalPoints++;

        if (totalDecimalPoints > 1)
            return false;

    }

    return true;
}

/* Utility function to decide whether a std::string is a whole point number or not */
bool String::isWholeNumber(const std::string &string) {

    if (string.empty())
        return false;

    if (string[0] != '+' && string[0] != '-' && !String::isDigit(string[0]))
        return false;


    for (unsigned int i = 1; i < string.size(); ++i)
        if (!String::isDigit(string[i]))
            return false;

    return true;
}


#include <fstream>
#include "Parser.h"
#include "../String/String.h"

/* Utility function to parse any file in Linux format that contains Curves in the following format : [curve_id y_1 y_2 ... y_n] */
std::vector<Curve *> *Parser::parseCurvesFile(const std::string &filePath, unsigned int dimensions, long double frequency, unsigned int complexity)
{

    /* Will only parse Curves as 1-dimensional or 2-dimensional */
    if (dimensions == 0 || dimensions > 2)
        return nullptr;

    std::ifstream file;
    file.open(filePath);

    std::vector<Curve *> *inputCurves = new std::vector<Curve *>;

    std::string line;

    /* Read each line of the file */
    while (std::getline(file, line)) {

        bool areTokensValid = true;

        /* Tokenize the line read */
        std::vector<std::string> *curveTokens = String::tokenizeString(line, '\t');

        /* Check if the line read has at least 3 tokens due to the fact that the provided Continuous-Frechet-Distance library works only for Curves with complexity of at least 2 */
        if (curveTokens->size() <= 2)
            areTokensValid = false;

        /* Check if each token besides the curve_id one is a floating point number */
        if (areTokensValid)
            for (long unsigned int i = 1; i < curveTokens->size(); ++i)
                if (!String::isFloatingPointNumber(curveTokens->at(i))) {
                    areTokensValid = false;
                    break;
                }

        /* If no complexity was provided then ONLY Curves with complexity X be parsed where X is the complexity of the first valid format-wise Curve */
        if (areTokensValid && complexity == 0)
            complexity = curveTokens->size() - 1;


        if (areTokensValid && curveTokens->size() - 1 == complexity) {

            /* Create a dimensions-dimensional Curve */
            Curve *curve = Curve::createCurve(curveTokens->at(0), dimensions);

            long double xCoordinate = 0.0;

            for (unsigned int i = 1; i < curveTokens->size(); ++i) {

                /* Create and insert an 1/2-dimensional Vector */
                std::vector<long double> coordinates;

                if (dimensions == 2) {
                    coordinates.push_back(xCoordinate);
                    xCoordinate += frequency;
                }

                coordinates.push_back(std::stold(curveTokens->at(i)));
                curve->insert(new Vector(coordinates));

            }

            inputCurves->push_back(curve);
        }

        delete curveTokens;
    }

    file.close();


    return inputCurves;
}



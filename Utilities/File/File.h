#ifndef FILE_H
#define FILE_H


#include <string>

class File {

public:
    static bool canOpenFile(const std::string &);
    static bool isFileEmpty(const std::string &);
    static bool isInWindowsFormat(const std::string &);
};


#endif

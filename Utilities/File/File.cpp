#include <fstream>
#include "File.h"

/* Utility function to check if a file can be opened */
bool File::canOpenFile(const std::string &filePath) {
    std::fstream file;
    file.open(filePath);
    bool canOpenFile = file.is_open();
    file.close();
    return canOpenFile;
}

/* Utility function to check if a file is empty */
bool File::isFileEmpty(const std::string &filePath) {
    std::fstream file;
    file.open(filePath);
    bool isEmpty = (file.peek() == std::fstream::traits_type::eof());
    file.close();
    return isEmpty;
}

/* Utility function to check if a file is in Windows format meaning its new line character set is "\r\n" */
bool File::isInWindowsFormat(const std::string &filePath) {

    std::fstream file;
    file.open(filePath);
    std::string line;
    std::getline(file, line);
    bool isInWindowsFormat = (line[line.size() - 1] == '\r');
    file.close();
    return isInWindowsFormat;
    
}

#include "StringTableReader.h"

void StringTableReader::setFilePath(char* filePath) { this->filePath = filePath; }

std::vector<std::vector<std::string>> StringTableReader::getTable() {
    char separator = ';';

    std::ifstream fileIn(filePath);
    size_t rowsSize, columnsSize;

    fileIn >> rowsSize >> columnsSize;
    std::vector<std::vector<std::string>> tableStrings;

    tableStrings.resize(rowsSize);
    for (auto& vec : tableStrings) vec.resize(columnsSize);

    for (size_t i = 0; i < rowsSize; i++) {
        for (size_t j = 0; j < columnsSize; j++) {
            getline(fileIn, tableStrings[i][j], separator);
            size_t lastInd = tableStrings[i][j].find('\n');
            if (lastInd < tableStrings[i][j].length()) tableStrings[i][j].erase(lastInd, 1);
        }
    }

    return tableStrings;
}

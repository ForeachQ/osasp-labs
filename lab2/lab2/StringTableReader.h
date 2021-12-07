#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>

class StringTableReader {
public:
    void setFilePath(char* filePath);
    std::vector<std::vector<std::string>> getTable();
private:
    char* filePath;
};
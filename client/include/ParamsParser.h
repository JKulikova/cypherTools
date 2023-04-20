#pragma once

#include <span>
#include <string>

class ParamsParser final{
    std::string algorithm;
    std::string filename;
public:
    bool isAlgorithmPresent();
    std::string getAlgorithm();
    void setAlgorithm(std::string);

    bool isFilenamePresent();
    std::string getFilename();
    void setFilename(std::string);

    static ParamsParser parse(std::span<const std::string>);
};

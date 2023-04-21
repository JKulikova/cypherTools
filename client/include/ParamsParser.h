#pragma once

#include <span>
#include <string>

class ParamsParser final{
    std::string algorithm;
    std::string filename;
    std::string lsbMessageInputOutput;
public:
    bool isAlgorithmPresent();
    std::string getAlgorithm();
    void setAlgorithm(std::string);

    bool isFilenamePresent();
    std::string getFilename();
    void setFilename(std::string);

    bool isLsbMessageInputOutputPresent();
    std::string getLsbMessageInputOutput();
    void setLsbMessageInputOutput(std::string);

    static ParamsParser parse(std::span<const std::string>);
};

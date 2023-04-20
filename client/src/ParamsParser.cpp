#include <ParamsParser.h>

#include <stdexcept>

bool ParamsParser::isAlgorithmPresent(){
    return !algorithm.empty();
}

std::string ParamsParser::getAlgorithm(){
    return algorithm;
}

void ParamsParser::setAlgorithm(std::string value){
    if(!algorithm.empty())
        throw std::logic_error { "multiple values of algorithm"  };
    algorithm = std::move(value);
}

bool ParamsParser::isFilenamePresent(){
    return !filename.empty();
}

std::string ParamsParser::getFilename(){
    return filename;
}

void ParamsParser::setFilename(std::string value){
    if(!filename.empty())
        throw std::logic_error { "multiple values of filename"  };
    filename = std::move(value);
}

ParamsParser ParamsParser::parse(std::span<const std::string> args){
    if (args.empty())
        throw std::invalid_argument { "no filename" };

    ParamsParser params;
    for (auto it = args.begin(); it != args.end(); ++it) {
        if (*it == "-a") {
            if (it + 1 == args.end())
                throw std::invalid_argument { "no value for -a" };
            params.setAlgorithm(*(++it));
        } else if (it->starts_with("-")) {
            throw std::invalid_argument { "unknown option: " + *it };
        } else {
            params.setFilename(*it);
        }
    }

    if (!params.isFilenamePresent())
        throw std::invalid_argument { "no filename" };

    return params;
}

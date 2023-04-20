#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <main.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        cypherTools::help();
        return 0;
    }

    const std::filesystem::path output{ cypherTools::outputSubdir };
    if(!std::filesystem::exists(output))
        std::filesystem::create_directory(output);

    std::vector<std::string> args { argv + 1, argv + argc };

    if (args[0] == "help" || args[0] == "-h" || args[0] == "--help") {
        cypherTools::help();
        return 0;
    }

    if (args[0] == "encode") {
        try {
            cypherTools::encode(std::span<const std::string> { args.cbegin() + 1, args.cend() });
        } catch (const std::exception& e) {
            std::cerr << "Fatal: " << e.what() << "." << std::endl;
            return 1;
        } catch (...) {
            std::cerr << "Fatal: unexpected error." << std::endl;
            return 1;
        }
        return 0;
    }

    if (args[0] == "decode") {
        try {
            cypherTools::decode(std::span<const std::string> { args.cbegin() + 1, args.cend() });
        } catch (const std::exception& e) {
            std::cerr << "Fatal: " << e.what() << "." << std::endl;
            return 1;
        } catch (...) {
            std::cerr << "Fatal: unexpected error." << std::endl;
            return 1;
        }
        return 0;
    }

    std::cerr << "Unknown command: " << argv[1] << "." << std::endl;
    return 0;
}

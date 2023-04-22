#include <iostream>
#include <main.h>

namespace {
    constexpr char helpMessage[] =
            "All encoded files are expected to be in ../output/ directory\n"
            "All products placed in ../output/ directory\n"
            "\n"
            "Command list:\n"
            "    encode -a rsa <filename>\n"
            "    encode -a lsb <filename.bmp> <message filename>\n"
            "\n"
            "    decode -a <algorithm> <filename>\n"
            "\n"
            "    help (-h | --help)\n";
} // namespace

void cypherTools::help() {
    std::cout << helpMessage << std::endl;
}

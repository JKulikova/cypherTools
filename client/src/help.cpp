#include <iostream>
#include <main.h>

namespace {
    constexpr char helpMessage[] =
            "Command list:\n"
            "    encode -a <algorithm> <filename>\n"
            "    decode -a <algorithm> <filename>\n"
            "    help (-h | --help)\n";
} // namespace

void cypherTools::help() {
    std::cout << helpMessage << std::endl;
}

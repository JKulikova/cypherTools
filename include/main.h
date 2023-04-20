#pragma once

#include <span>
#include <cstdint>
#include <string>

namespace cypherTools {
    constexpr char fileExtension[] = ".ctools";
    constexpr char outputSubdir[] = "output/";

    void encode(std::span<const std::string> args);
    void decode(std::span<const std::string> args);

    void help();
} // namespace cypherTools

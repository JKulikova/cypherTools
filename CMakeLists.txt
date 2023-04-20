cmake_minimum_required(VERSION 3.21)
project(cypherTools)

set(CMAKE_CXX_STANDARD 20)


# Targets
add_library(
        lib
        ""
)

add_executable(
        cypherTools
        ""
)

# Common include files
target_include_directories(
        lib
        PUBLIC
            include
)

target_include_directories(
        cypherTools
        PUBLIC
            include
)

# Targets dependencies
target_link_libraries(
        cypherTools
        PUBLIC
            lib
)

# Targets sources
add_subdirectory(lib)
add_subdirectory(client)

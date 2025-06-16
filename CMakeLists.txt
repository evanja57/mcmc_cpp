cmake_minimum_required(VERSION 3.10)
project(Deciphering)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# Add compiler optimization flag for speed
add_compile_options(-O3)

# Include directories
include_directories(include)

# Add library
add_library(decipher_lib src/utils.cpp src/deciphering_utils.cpp src/metropolis_hastings.cpp)

# Add executables
add_executable(run_deciphering src/run_deciphering.cpp)
add_executable(scramble_text src/scramble_text.cpp)

# Add a custom target to run clang-format
file(GLOB_RECURSE ALL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
)

add_custom_target(format
    COMMAND clang-format -i ${ALL_SOURCE_FILES}
    COMMENT "Formatting C++ files with clang-format"
)

# Link libraries
target_link_libraries(run_deciphering decipher_lib Threads::Threads)
target_link_libraries(scramble_text decipher_lib)
cmake_minimum_required(VERSION 3.22)
project(WynnCraftingOptimizer)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(ingredient-parser)

add_executable(WynnCraftingOptimizer main.cpp)
add_dependencies(WynnCraftingOptimizer generate_header)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/ingredient-parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shared)
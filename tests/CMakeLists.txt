cmake_minimum_required(VERSION 3.10)
project(visualization.matrix.tests)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GoogleTest
find_package(GTest REQUIRED)
find_package(GTestMain REQUIRED)

# Include parent project for dependencies
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib/kissfft
)

# Add test executable
add_executable(test_visualization_matrix
    test_visualization_matrix.cpp
)

# Link GoogleTest and required libraries
target_link_libraries(test_visualization_matrix
    GTest::GTest
    GTest::Main
    m  # For math functions (log10f, sin, cos, etc.)
)

# Enable testing
enable_testing()

# Add test to CTest
add_test(NAME test_visualization_matrix COMMAND test_visualization_matrix)

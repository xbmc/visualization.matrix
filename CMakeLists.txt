cmake_minimum_required(VERSION 3.20)
project(visualization.matrix VERSION 1.0.0 LANGUAGES CXX)

# Project metadata
set(PROJECT_DESCRIPTION "Matrix themed visualization addon for Kodi")
set(PROJECT_AUTHOR "Team Kodi")
set(PROJECT_LICENSE "GPL-2.0-or-later")

# Set C++ standard (C++17 for Kodi Matrix compatibility)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set CMake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

# Options
option(BUILD_TESTING "Build tests" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck" OFF)

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

# Fetch kissfft dependency
include(FetchContent)
FetchContent_Declare(
  kissfft
  GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(kissfft)

# Find required packages
find_package(Kodi REQUIRED)
find_package(glm REQUIRED)

# Platform-specific OpenGL handling
if(NOT WIN32 AND (APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM))
  find_package(OpenGl REQUIRED)
  set(DEPLIBS ${OPENGL_LIBRARIES})
  set(includes ${OPENGL_INCLUDE_DIR})
  add_definitions(${OPENGL_DEFINITIONS})
else()
  find_package(OpenGLES REQUIRED)
  set(DEPLIBS ${OPENGLES_LIBRARIES})
  set(includes ${OPENGLES_INCLUDE_DIR})
  add_definitions(${OPENGLES_DEFINITIONS})
endif()

# Include directories
include_directories(
  ${GLM_INCLUDE_DIR}
  ${KODI_INCLUDE_DIR}/..
  ${PROJECT_SOURCE_DIR}/lib
  ${kissfft_SOURCE_DIR}
)

# Platform-specific libraries
if(CORE_SYSTEM_NAME STREQUAL osx OR
   CORE_SYSTEM_NAME STREQUAL ios OR
   CORE_SYSTEM_NAME STREQUAL darwin_embedded)
  list(APPEND DEPLIBS "-framework CoreVideo")
endif()

# Source files
set(MATRIX_SOURCES
  src/main.cpp
  ${kissfft_SOURCE_DIR}/kiss_fft.c
)

set(MATRIX_HEADERS
  src/main.h
  src/stb_image.h
  ${kissfft_SOURCE_DIR}/kiss_fft.h
  ${kissfft_SOURCE_DIR}/kiss_fft_log.h
  ${kissfft_SOURCE_DIR}/_kiss_fft_guts.h
)

# Create kissfft library target
add_library(kissfft STATIC
  ${kissfft_SOURCE_DIR}/kiss_fft.c
  ${kissfft_SOURCE_DIR}/kiss_fft.h
  ${kissfft_SOURCE_DIR}/kiss_fft_log.h
  ${kissfft_SOURCE_DIR}/_kiss_fft_guts.h
)
target_include_directories(kissfft PUBLIC ${kissfft_SOURCE_DIR})

# Main visualization addon
add_library(visualization.matrix MODULE
  ${MATRIX_SOURCES}
  ${MATRIX_HEADERS}
)

# Link libraries
target_link_libraries(visualization.matrix PRIVATE
  kissfft
  ${DEPLIBS}
  ${KODI_LIBRARIES}
)

# Set target properties
target_compile_options(visualization.matrix PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Install target
install(TARGETS visualization.matrix
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
)

# CPack configuration
include(CPack)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR ${PROJECT_AUTHOR})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.md)

# Enable testing if requested
if(BUILD_TESTING)
  enable_testing()
  # Add tests here when available
endif()

# Code coverage
if(ENABLE_COVERAGE)
  target_compile_options(visualization.matrix PRIVATE --coverage)
  target_link_options(visualization.matrix PRIVATE --coverage)
endif()

# Clang-tidy integration
if(ENABLE_CLANG_TIDY AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set_target_properties(visualization.matrix PROPERTIES
    CXX_CLANG_TIDY "clang-tidy;-checks=*;-warnings-as-errors=*"
  )
endif()

# ----------------------
# Carousel Core Library
# -----------------------------------------------------------------------------
# Developer: Sebastian Carrion
# Year: 2024
# 
# Brief: This CMake file is responsible for setting up the build environment
#        for the carousel core library. It includes configurations for compiling
#        source files, linking necessary libraries, and setting up installation
#        paths.
# -----------------------------------------------------------------------------
cmake_minimum_required (VERSION 3.8)
project(CarouselCore VERSION 1.0.0 DESCRIPTION "CAROUSEL framework core library")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use conan package manager
# run: conan install . -s build_type=Debug --build missing
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

#include conan generators
include(build/Debug/generators/conan_toolchain.cmake)

# Lua scripting is used for extending the application at post production
find_package(lua REQUIRED)

# XercesC is used for generating datamodels based on xml
find_package(XercesC REQUIRED)

# Testing framework
find_package(Catch2 REQUIRED)

# Database engine
find_package(SQLite3 REQUIRED)

# Logger service
find_package(log4cplus REQUIRED)

# Poco
find_package(Poco REQUIRED)

# Define environment XSD_INCLUDE_DIRS
set(XSD_INCLUDE_DIRS $ENV{XSD_INCLUDE_DIRS} CACHE PATH "Path to XSD include directory")
set(XSD_BIN $ENV{XSD_BIN} CACHE PATH "Path to XSD include directory")
message("XSD bin path is: ${XSD_BIN}")
message("XSD include path is: ${XSD_INCLUDE_DIRS}")

include_directories(${XSD_INCLUDE_DIRS})
add_subdirectory("Carousel")

# Core library
file(GLOB_RECURSE CAROUSEL_DATA_MODELS_H "Carousel/*.h")
file(GLOB_RECURSE CAROUSEL_DATA_MODELS_CPP "Carousel/*.cpp")
add_library(${PROJECT_NAME} STATIC ${CAROUSEL_DATA_MODELS_H} ${CAROUSEL_DATA_MODELS_CPP})
target_link_libraries(${PROJECT_NAME} Catch2::Catch2WithMain SQLite::SQLite3 XercesC::XercesC lua::lua log4cplus::log4cplus Poco::Poco)
# target_include_directories(${PROJECT_NAME} PRIVATE "Carousel")

add_subdirectory("Tests")




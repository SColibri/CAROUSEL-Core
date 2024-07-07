# ----------------------
# Data models
# -----------------------------------------------------------------------------
# Developer: Sebastian Carrion
# Year: 2024
# 
# Brief: Based on xml schemas, shared data models are generated using xsd
#        generator for c++. Run this cmake script when updating the schema
#        and don't forget to update any other interfaces that require these
#        shared data models.
# -----------------------------------------------------------------------------
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
project(CarouselDataModels VERSION 1.0.0 DESCRIPTION "CAROUSEL framework data models")

# Incude xsd functions
include(xsdFunctions.cmake)

# Generate files using xsd
set(SCHEMAS_DIR Data/Schemas)
generate_files(${SCHEMAS_DIR}/carouselModels.xsd ${CMAKE_CURRENT_SOURCE_DIR}/Data/SharedTypes)
generate_files(${SCHEMAS_DIR}/CarouselConfigurations.xsd ${CMAKE_CURRENT_SOURCE_DIR}/Data/SharedTypes)

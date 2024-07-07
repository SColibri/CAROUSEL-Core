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

# Add the schemas
set(SCHEMAS_DIR schemas)
set(SCHEMA_FILES ${SCHEMAS_DIR}/carouselModels.xsd)
set(SCHEMA_FILE_OUT ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Required Tools: XSD
execute_process( 
    COMMAND ${XSD_BIN}
         cxx-tree
         --std=c++17
         --type-naming knr
         --hxx-suffix .h
         --cxx-suffix .cpp
         --generate-serialization
         --generate-ostream
         --generate-comparison
         --root-element-all
         --output-dir ${SCHEMA_FILE_OUT}
         ${SCHEMA_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE XSD_RESULT
    OUTPUT_VARIABLE XSD_OUTPUT
    ERROR_VARIABLE XSD_ERROR
)

if(NOT XSD_RESULT EQUAL 0)
    message(FATAL_ERROR "XSD generation failed with error: ${XSD_ERROR}")
else()
    message(STATUS "XSD generation output: ${XSD_OUTPUT}")
endif()

# Before build create dependencies
add_custom_target(generate_xsd_files ALL
    DEPENDS ${SCHEMA_FILE_OUT}/carouselModels.h ${SCHEMA_FILE_OUT}/carouselModels.cpp
)


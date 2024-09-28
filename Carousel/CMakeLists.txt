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
set(SCHEMAS_DIR include/Data/Schemas)
generate_files(${SCHEMAS_DIR}/carouselModels.xsd ${CMAKE_CURRENT_SOURCE_DIR}/include/Data/SharedTypes)
generate_files(${SCHEMAS_DIR}/CarouselConfigurations.xsd ${CMAKE_CURRENT_SOURCE_DIR}/include/Data/SharedTypes)

set(SCRIPTING_DIR include/Scripting)
execute_process(
	COMMAND Python3 ${CMAKE_CURRENT_SOURCE_DIR}/pythonBindings.py 
					${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPTING_DIR}
					carouseldata
					${CMAKE_CURRENT_SOURCE_DIR}/include/Data/Models/*.h
					${CMAKE_CURRENT_SOURCE_DIR}/include/Core/BaseTypes/CarouselSandbox.h
					${CMAKE_CURRENT_SOURCE_DIR}/include/Logging/CarouselLogger.h
					${CMAKE_CURRENT_SOURCE_DIR}/include/IpcTools/IpcPipeHandler.h
	RESULT_VARIABLE bindingResult
)

# Check the result of execute_process
if(NOT bindingResult EQUAL 0)
    message(FATAL_ERROR "Python script execution failed with result: ${bindingResult}")
endif()

# TODO: check if creating the module is require in future
# pybind11_add_module(carouselModule Include/Scripting/CarouselPythonCore.cpp)
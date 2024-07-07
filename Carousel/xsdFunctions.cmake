# ----------------------
# Data models
# -----------------------------------------------------------------------------
# Developer: Sebastian Carrion
# Year: 2024
# 
# Brief: Generates .cpp and .h files using xsd code generator
# -----------------------------------------------------------------------------
function(generate_files schema_file output_directory)

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
             --output-dir ${output_directory}
             ${schema_file}
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

endfunction()
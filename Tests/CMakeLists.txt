enable_testing()

# Directory that contains the tests
set(TEST_DIRECTORY ${PROJECT_SOURCE_DIR}/Tests)

# Get list of all files
file(GLOB TEST_FILES ${TEST_DIRECTORY}/*.cpp)

# Create all tests
message("Creating tests for: ${fname}")
foreach(target IN LISTS TEST_FILES)
    
    # Get test name
    get_filename_component(fname ${target} NAME)
    string(REGEX REPLACE ".cpp" "" fname ${fname})

    # create tests
    message("Looking into filename: ${fname}")
    add_executable(${fname} "${target}")
	set_target_properties(${fname} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_features(${fname} PUBLIC cxx_std_17)
    target_compile_options(${fname} PRIVATE ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
    target_link_libraries(${fname} CarouselCore Catch2::Catch2WithMain SQLite::SQLite3 XercesC::XercesC lua::lua)

    add_test(NAME ${fname} COMMAND ${fname})
endforeach()

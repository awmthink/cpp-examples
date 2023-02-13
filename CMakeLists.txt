cmake_minimum_required(VERSION 3.22)
project(cpp_exampels)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(examples)
add_subdirectory(tests)

set(CLANG_SEARCH_PATH "/usr/bin/" "/usr/local/bin" "$ENV{LLVM_ROOT_DIR}/bin")
set(TOOLS_PATH "${CMAKE_SOURCE_DIR}/tools")

##########################################
# "make format"
# "make check-format"
##########################################

# clang format
find_program(CLANG_FORMAT_BIN
        NAMES clang-format-10 clang-format
        HINTS ${CLANG_SEARCH_PATH})
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Couldn't find clang-format")
else()
    message(STATUS "Found clang-format at ${CLANG_FORMAT_BIN}")
endif()

string(CONCAT FORMAT_DIRS
    "${CMAKE_SOURCE_DIR}/examples,"
    "${CMAKE_SOURCE_DIR}/tests"
)

# runs clang format and updates files in place.
add_custom_target(format ${TOOLS_PATH}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${TOOLS_PATH}/clang_format_exclusions.txt
        --source_dirs ${FORMAT_DIRS}
        --fix
        --quiet
)
# runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${TOOLS_PATH}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${TOOLS_PATH}/clang_format_exclusions.txt
        --source_dirs ${FORMAT_DIRS}
        --quiet
)

###########################################################
# "make check-clang-tidy" target
###########################################################

# clang-tidy
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy-10 clang-tidy
        HINTS ${CLANG_SEARCH_PATH})
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "Couldn't find clang-tidy")
else()
    message(STATUS "Found clang-tidy at ${CLANG_TIDY_BIN}")
endif()

# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(check-clang-tidy
        ${TOOLS_PATH}/run_clang_tidy.py
        -clang-tidy-binary ${CLANG_TIDY_BIN}
        -p ${CMAKE_BINARY_DIR}
        )

add_custom_target(check-all)
add_dependencies(check-all check-format check-clang-tidy)

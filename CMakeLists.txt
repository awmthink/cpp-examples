cmake_minimum_required(VERSION 3.16)
project(cpp_exampels)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.10.0
)
FetchContent_MakeAvailable(spdlog)

find_package(Threads REQUIRED)

file(GLOB_RECURSE src_tests ${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.cc ${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cc)
add_executable(cpp_tests ${src_tests})
target_include_directories(cpp_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(cpp_tests GTest::gtest_main spdlog::spdlog ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(cpp_tests PRIVATE -Wall -Wextra -Wpedantic -Wfloat-equal)
target_compile_options(cpp_tests PRIVATE $<$<CONFIG:Debug>:-Os -g3 -fsanitize=undefined,address,leak>)
target_link_options(cpp_tests PRIVATE $<$<CONFIG:Debug>:-fsanitize=undefined,address,leak>)

add_subdirectory(playground)

include(GoogleTest)
gtest_discover_tests(cpp_tests)

enable_testing()
add_test(NAME cpp_tests COMMAND cpp_tests)

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
    "${CMAKE_SOURCE_DIR}/src,"
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
add_dependencies(check-clang-tidy cpp_tests)

add_custom_target(check-all)
add_dependencies(check-all check-format check-clang-tidy)


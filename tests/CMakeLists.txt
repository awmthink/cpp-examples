include(FetchContent) 
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)
enable_testing()

file(GLOB test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cc)
add_executable(tests ${test_srcs})
add_test(NAME tests COMMAND tests)
target_link_libraries(tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)
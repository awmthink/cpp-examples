include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(autodiff ${CMAKE_CURRENT_SOURCE_DIR}/autodiff_test.cc)
target_link_libraries(autodiff GTest::gtest_main)
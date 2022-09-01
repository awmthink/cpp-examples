#include <gtest/gtest.h>

#include <iostream>

TEST(HelloTest, PrintHello) { std::cout << "hello, world" << std::endl; }

TEST(HelloTest, PrintCXXStandard) {
  std::cout << "c++ standard: " << __cplusplus << std::endl;
}

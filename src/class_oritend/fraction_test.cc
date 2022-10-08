#include "class_oritend/fraction.h"

#include <gtest/gtest.h>

TEST(ClassOritendTest, FractionTest) {
  Fraction f(1, 2);
  auto d = static_cast<double>(f);
  std::cout << d << std::endl;
}

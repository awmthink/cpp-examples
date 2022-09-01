#include <gtest/gtest.h>

#include <iostream>

TEST(TypesTest, Intergers) {
  // NOLINTBEGIN
  EXPECT_EQ(sizeof(char), 1);
  EXPECT_GE(sizeof(short), 2);
  EXPECT_GE(sizeof(int), sizeof(short));
  EXPECT_GE(sizeof(long), sizeof(int));
  EXPECT_GE(sizeof(long), 4);
  EXPECT_GE(sizeof(long long), sizeof(long));
  EXPECT_GE(sizeof(long long), 8);

  int i = 2;
  float f = 2.0F;
  double d = 2.0;

  std::cout << i << std::endl;
  std::cout << f << std::endl;
  std::cout << d << std::endl;
  // NOLINTEND
}

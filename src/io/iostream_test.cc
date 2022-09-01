#include <gtest/gtest.h>

#include <cstdint>
#include <sstream>

TEST(IOTest, ReadException) {
  int32_t i{0};
  float f{.0F};

  std::istringstream iss_norm("  123  1e-3");
  // 从第一个非空字符开始读取，到第一个非数字结束
  iss_norm >> i;
  // 从第一个非空字符开始读取，支持小数表示以及科学计数法表示
  iss_norm >> f;
  EXPECT_EQ(123, i);
  EXPECT_FLOAT_EQ(f, 1e-3);

  std::istringstream iss_bad_int(" x123  1e-3");
  iss_bad_int >> i;
  EXPECT_FALSE(iss_bad_int);

  std::istringstream iss_bad_float("e-3");
  iss_bad_float >> f;
  EXPECT_FALSE(iss_bad_float);
}

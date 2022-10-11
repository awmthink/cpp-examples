#include "auto_diff/auto_diff.h"

#include <gtest/gtest.h>

TEST(AutoDiff, UnaryOperators) {
  auto v0 = ad::Variable{2};
  auto v1 = v0.Sin();
  auto v2 = v1.Log();
  auto v3 = -v2;
  EXPECT_NEAR(0.0950830, v3.Value(), 1e-5);
  v3.Backpropagation();
  EXPECT_NEAR(0.4576575, v0.GetAdjoint().Value(), 1e-5);
}

TEST(AutoDiff, BinaryOperators) {
  auto v0 = ad::Variable{2};
  auto v1 = ad::Variable{5};
  auto v2 = v0.Log();
  auto v3 = v0 * v1;
  auto v4 = v1.Sin();
  auto v5 = v2 + v3;
  auto v6 = v5 - v4;

  v6.Backpropagation();
  EXPECT_NEAR(11.652071, v6.Value(), 1e-5);
  EXPECT_NEAR(5.5, v0.GetAdjoint().Value(), 1e-5);
  EXPECT_NEAR(1.716338, v1.GetAdjoint().Value(), 1e-5);
}

TEST(AutoDiff, ComposeOperators) {
  auto v0 = ad::Variable{2};
  auto v1 = ad::Variable{5};

  auto v2 = v0.Log();
  auto v3 = v0 * v1;
  auto v4 = v1.Sin();
  auto v5 = v2 + v3;
  auto v6 = v5 - v4;
  v6.Backpropagation();

  auto v7 = v0.Log() + v0 * v1 - v1.Sin();
  EXPECT_FLOAT_EQ(v6.Value(), v7.Value());

  ad::Variable::ZeroGradient();
  v7.Backpropagation();
  EXPECT_FLOAT_EQ(v6.GetAdjoint().Value(), v7.GetAdjoint().Value());
}

TEST(AutoDiff, MultiPath) {
  auto v0 = ad::Variable{2};
  auto v1 = ad::Variable{5};

  auto v2 = v0.Log() * v1;  // 1/v0 * v1,  v0.log
  auto v3 = v0.Sin() - v1;  // vo.cos, -1

  v2.Backpropagation();
  v3.Backpropagation();

  EXPECT_FLOAT_EQ(5.0F / 2 + std::cos(2), v0.GetAdjoint().Value());
  EXPECT_FLOAT_EQ(std::log(2) - 1, v1.GetAdjoint().Value());
}

TEST(AutoDiff, GetAdjoint) {
  auto v = ad::Variable{2};
  EXPECT_ANY_THROW(v.GetAdjoint());
}
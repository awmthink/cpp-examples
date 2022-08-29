#include <gtest/gtest.h>

#include <deque>

TEST(ContainersTest, DequeInit) {
  std::deque d = {1, 2, 3, 4, 5};
  EXPECT_EQ(d.size(), 5);
  std::vector<int> v = {1, 2, 3, 4, 5};
  std::deque d1(v.begin(), v.end());
  EXPECT_EQ(d1.size(), 5);
}

TEST(ContainersTest, InsertValue) {
  std::deque d = {1, 2, 3, 4, 5};
  d.push_back(6);
  EXPECT_EQ(6, d.size());
  d.pop_back();
  EXPECT_EQ(d.size(), 5);
  d.push_front(0);
  EXPECT_EQ(6, d.size());
  d.pop_front();
  EXPECT_EQ(d.size(), 5);
}

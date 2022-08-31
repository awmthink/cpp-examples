#include "template/distance.h"

#include <gtest/gtest.h>

#include <iterator>
#include <list>
#include <set>
#include <vector>

TEST(TemplateTest, DistanceTest) {
  std::vector<int> v = {1, 2, 3, 4, 5};
  EXPECT_EQ(5, Distance(v.begin(), v.end()));

  std::list<int> l = {1, 2, 3, 4, 5, 6};
  EXPECT_EQ(6, Distance(l.begin(), l.end()));

  std::set<int> s = {1, 2};
  EXPECT_EQ(2, Distance(s.begin(), s.end()));

  int* arr = new int[3];
  EXPECT_EQ(3, Distance(arr, arr + 3));
  delete[] arr;
}

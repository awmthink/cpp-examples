#include "iterator/line_iterator.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>

TEST(IteratorTest, LineIteratorForRange) {
  std::istringstream iss("first line\nsecond line\nthird line");
  LineReader reader(iss);
  for (const auto &line : reader) {
    std::cout << line << std::endl;
  }
}

TEST(IteratorTest, LineIteratorMultiBegin) {
  std::istringstream iss("first line\nsecond line\nthird line");
  LineReader reader(iss);
  auto begin = reader.begin();
  for (auto iter = reader.begin(); iter != reader.end(); ++iter) {
    std::cout << *iter << std::endl;
  }
}

TEST(IteratorTest, CopyToOstream) {
  std::istringstream iss("first line\nsecond line\nthird line");
  LineReader reader(iss);
  std::copy(reader.begin(), reader.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
}

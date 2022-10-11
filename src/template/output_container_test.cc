#include "template/output_container.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(TemplateTest, OutputContainerTest) {
  std::vector<std::vector<std::pair<std::string, std::string>>> aa = {
      {{"a", "a1"}, {"b", "b1"}, {"c", "c1"}}, {{"d", "d1"}, {"e", "e1"}, {"f", "f1"}}};

  std::cout << aa << std::endl;
}

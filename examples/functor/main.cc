#include <functional>
#include <iostream>
#include <utility>

struct Plus {
 public:
  int operator()(int a, int b) { return a + b; }
};
int main() {
  Plus adder;
  std::cout << adder(1, 2) << std::endl;
  auto adder_1 = std::bind(adder, std::placeholders::_1, 1);
  return 0;
}
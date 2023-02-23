#include <algorithm>
#include <functional>
#include <iostream>
#include <ranges>
#include <string>

struct User {
  std::string name;
  int age;
};

bool underage(const User &user) { return user.age < 18; }

int main() {
  const std::vector<User> users = {User{"A", 19}, User{"B", 20}, User{"C", 17}};
  auto result =
      users | std::views::filter(std::not_fn(underage)) |
      std::views::transform([](const auto &user) { return user.name; });
  std::ranges::copy(result,
                    std::ostream_iterator<std::string>(std::cout, "\n"));
};

// dlsys -> d2l -> lihongyi
// 
// 下午：monolith

// cuda -> kestrel&flock -> triton -> taskflow -> faiss
// 晚上：review notes（3） -> effective modern cpp（4&5） -> best practices（6） -> clean（7&8） -> profiling（9） -> 周末花1-2个小时进行整理和验证

// golang & backend master & iam
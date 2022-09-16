#include <gtest/gtest.h>

template <typename T>
void Print(T arg) {
  std::cout << arg;
}

template <typename T, typename... Args>
void Print(T arg, Args... args) {
  std::cout << arg << ", ";
  Print(args...);
}

// PrintBetter 使用了`if constexptr`来进行编译期条件判断
// 在这种情况下就不需要像上面的`Print`那样写一个单参数的重载版本
template <typename T, typename... Args>
void PrintBetter(T arg, Args... args) {
  std::cout << arg << ", ";
  if constexpr (sizeof...(args) > 0) {
    PrintBetter(args...);
  }
}

TEST(TemplateTest, VariadicTest) {
  int a = 0;
  float pi = 3.14F;
  std::string str = "hello, world";
  const float *p = &pi;
  PrintBetter(a, pi, str, p);
  std::cout << std::endl;
}

template <typename... T>
auto FoldSum(T... s) {
  return (... + s);  // 括号里是一个折叠表达式
}

// 利用折叠表达式的版本
template <typename... Args>
void FoldPrint(Args const &... args) {
  (std::cout << ... << args) << "\n";
}

template <typename FirstType, typename... Args>
void FoldPrint(FirstType first, Args... args) {
  std::cout << first;

  auto print_white_space = [](const auto arg) { std::cout << " " << arg; };

  (..., print_white_space(args));
}

TEST(TemplateTest, FoldExpressions) { EXPECT_EQ(10, FoldSum(1, 2, 3, 4)); }

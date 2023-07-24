#include <iostream>
#include <string_view>

template <typename T>
constexpr auto GetTypeName() {
  constexpr auto prefix = std::string_view{"[with T = "};
  constexpr auto suffix = "]";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};

  const auto start = function.find(prefix) + prefix.size();
  const auto end = function.find_last_of(suffix);
  const auto size = end - start;

  return function.substr(start, size);
}

int main() {
  int x1 = 42;
  const int &x2 = x1;
  int &&x3 = std::move(x1);

  const int *const volatile p = &x1;

  std::cout << GetTypeName<decltype(x1)>() << std::endl;  // int
  std::cout << GetTypeName<decltype(x2)>() << std::endl;  // const int&
  std::cout << GetTypeName<decltype(x3)>() << std::endl;  // int&&
  std::cout << GetTypeName<decltype(p)>()
            << std::endl;  // const int* const volatile

  auto f = [](int x) { return x; };
  std::cout << GetTypeName<decltype(f)>()
            << std::endl;  // main()::<lambda(int)>
  return 0;
}
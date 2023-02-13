#include <iostream>
#include <span>
#include <string_view>
#include <vector>

#include "lib.h"

void PrintStr(std::string_view str) { std::cout << str << std::endl; }

void PrintArray(std::span<int> sp) {
  for (auto n : sp) {
    std::cout << n << std::endl;
  }
}

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

template <typename T>
void PrintTypeName(T&& arg) {
  std::cout << "type(T): " << GetTypeName<T>() << ", type(arg)："
            << GetTypeName<decltype(arg)>() << std::endl;
}

template <typename T>
void f(T&& arg) {
  PrintTypeName(arg);
  PrintTypeName(std::decay_t<T>(arg));
}

int main(int argc, char* argv[]) {
  f("Hello");
  return 0;
}


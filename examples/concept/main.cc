#include <cmath>
#include <concepts>
#include <iostream>
#include <type_traits>

template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

template <typename T>
concept hashable = requires(T t) {
  { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

template <typename T>
  requires std::floating_point<T>
bool IsPowerOf2(T x) {
  int exponent;
  const T maintissa = std::frexp(x, &exponent);
  return maintissa == T(0.5);
}

template <std::integral T>
bool IsPowerOf2(T x) {
  return x > 0 && (x & (x - 1)) == 0;
}

int main() {
  std::cout << IsPowerOf2(1024) << std::endl;
  std::cout << IsPowerOf2(0.25) << std::endl;
}
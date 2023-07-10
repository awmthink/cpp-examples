// sample to show std::variant usage
#include <iostream>
#include <variant>

int main() {
  std::variant<int, float> v, w;
  v = 12;  // v contains int
  int i = std::get<int>(v);
  w = std::get<int>(v);
  w = std::get<0>(v);  // same effect as the previous line
  w = v;               // same effect as the previous line

  try {
    std::get<float>(w);  // w contains int, not float: will throw
  } catch (const std::bad_variant_access&) {
  }

  return 0;
}
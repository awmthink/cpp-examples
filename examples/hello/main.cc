
#include <chrono>
#include <complex>
#include <iostream>
#include <string>
#include <thread>

struct length {
  double value;
  enum unit {
    metre,
    kilometre,
    millimetre,
    centimetre,
    inch,
    foot,
    yard,
    mile,
  };
  static constexpr double factors[] = {1.0,    1000.0, 1e-3,   1e-2,
                                       0.0254, 0.3048, 0.9144, 1609.344};
  explicit length(double v, unit u = metre) { value = v * factors[u]; }
};



length operator+(length lhs, length rhs) {
  return length(lhs.value + rhs.value);
}

// 可能有其他运算符

int main() {
  using namespace std::literals::complex_literals;
  std::cout << "i * i = " << 1i * 1i << std::endl;

  using namespace std::literals::chrono_literals;
  std::cout << "Waiting for 500ms" << std::endl;
  std::this_thread::sleep_for(500ms);

  using namespace std::literals::string_literals;
  std::cout << "Hello world"s.substr(0, 5) << std::endl;
}
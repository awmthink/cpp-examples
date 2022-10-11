#include <gtest/gtest.h>

class Shape {};
class Circle : public Shape {};

// NOLINTNEXTLINE
void Foo(const Shape &) { std::cout << "Foo(const Shape &)" << std::endl; }

// NOLINTNEXTLINE
void Foo(const Shape &&) { std::cout << "Foo(const Shape &&)" << std::endl; }

void Bar(const Shape &s) {
  std::cout << "Bar(const Shape &)" << std::endl;
  Foo(s);
}

void Bar(const Shape &&s) {
  std::cout << "Bar(const Shape &&)" << std::endl;
  Foo(s);
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
void PrintTypeName(T &&arg) {
  std::cout << "type(T): " << GetTypeName<T>() << ", type(arg)："
            << GetTypeName<decltype(arg)>() << std::endl;
}

TEST(ForwardTest, ForwardARValue) { Bar(Circle{}); }

TEST(ForwardTest, ForwardAMovedRValue) { Bar(Circle{}); }

TEST(ForwardTest, PrintType) {
  int lvalue = 0;
  PrintTypeName(lvalue);  // int&,int&
  int &&rvalueref = 0;
  PrintTypeName(rvalueref);        // int&,int&
  PrintTypeName(3.14F);            // float, float&&
  PrintTypeName(std::string{""});  // std::__cxx11::basic_string<char>,
                                   // std::__cxx11::basic_string<char>&&
  const int ci = 1;
  PrintTypeName(ci);  // const int&, const int&
  // NOLINTNEXTLINE
  PrintTypeName(std::move(lvalue));  // int,int&&
}

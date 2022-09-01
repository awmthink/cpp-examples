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

TEST(ForwardTest, ForwardARValue) { Bar(Circle{}); }

TEST(ForwardTest, ForwardAMovedRValue) { Bar(Circle{}); }

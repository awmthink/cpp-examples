#include <gtest/gtest.h>

#include <iostream>

class Foo {
 public:
  virtual void f(int x) { std::cout << "Foo:f(int): " << x << std::endl; }
  // overloading：重载
  void f(double x) { std::cout << "Foo:f(double): " << x << std::endl; }
};
class Bar : public Foo {
 public:
  // overriding：虚函数重写
  void f(int x) override { std::cout << "Bar:f: " << x << std::endl; }
  // hiding: 隐藏了父类的实现
  void f(double x) { std::cout << "Bar:f(double): " << x << std::endl; }
};

TEST(OverloadTest, Overloading) {
  Foo *foo = new Bar;
  foo->f(42);    // Bar:f: 42
  foo->f(42.0);  // Foo:f(double): 42
}
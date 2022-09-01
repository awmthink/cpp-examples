#include <gtest/gtest.h>

#include <iostream>

class Foo {
 protected:
  int i_ = 0;
  double d_ = .0;

 public:
  virtual void Fun(int x) {
    i_ = x;
    std::cout << "Foo:Fun(int): " << x << std::endl;
  }
  // overloading：重载
  void Fun(double x) {
    d_ = x;
    std::cout << "Foo:Fun(double): " << x << std::endl;
  }
  virtual ~Foo() = default;
};
class Bar : public Foo {
 public:
  // overriding：虚函数重写
  void Fun(int x) override {
    i_ = x;
    std::cout << "Bar:Fun: " << x << std::endl;
  }
  // hiding: 隐藏了父类的实现
  void Fun(double x) {
    d_ = x;
    std::cout << "Bar:Fun(double): " << x << std::endl;
  }
};

TEST(OverloadTest, Overloading) {
  Foo *foo = new Bar;
  foo->Fun(42);    // Bar:f: 42
  foo->Fun(42.0);  // Foo:f(double): 42
  delete foo;
}

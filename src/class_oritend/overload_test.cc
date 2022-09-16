#include <gtest/gtest.h>

#include <iostream>

class Foo {
 protected:
  int i_ = 0;
  double d_ = .0;

 public:
  // NOLINTNEXTLINE
  virtual void Fun(int x) {
    i_ = x;
    std::cout << "Foo:Fun(int): " << x << std::endl;
  }
  // overloading：重载
  void Fun(double x) {
    d_ = x;
    std::cout << "Foo:Fun(double): " << x << std::endl;
  }

  void NonVirtualFun(int x) {
    i_ = x;
    std::cout << "Foo:NonVirtualFun(int): " << x << std::endl;
  }

  virtual ~Foo() = default;
};
class Bar : public Foo {
 public:
  // hiding: 隐藏了父类的实现
  // NOLINTNEXTLINE
  void Fun(double x) {
    d_ = x;
    std::cout << "Bar:Fun(double): " << x << std::endl;
  }

  // 隐藏父类中的所有同名函数，即使参数不同
  void NonVirtualFun(double d) {
    d_ = d;
    std::cout << "Foo:NonVirtualFun(double): " << d << std::endl;
  }
};

TEST(OverloadTest, Overloading) {
  Foo *foo = new Bar;
  foo->Fun(42);    // Bar:f: 42
  foo->Fun(42.0);  // Foo:f(double): 42
  delete foo;
}

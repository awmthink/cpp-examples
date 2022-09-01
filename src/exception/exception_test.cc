#include <gtest/gtest.h>

#include <exception>
#include <iostream>

class TestExcetpion {
  int32_t i_ = 0;

 public:
  TestExcetpion() { std::cout << "generate a TestExcetpion" << std::endl; }
  TestExcetpion(const TestExcetpion& te) {
    i_ = te.i_;
    std::cout << "copy a testexception" << std::endl;
  }
  TestExcetpion(TestExcetpion&& te) noexcept {
    i_ = te.i_;
    std::cout << "move a testexception" << std::endl;
  }
  ~TestExcetpion() = default;
};

class A {
 public:
  void Print() {}
  ~A() { std::cout << "~A" << std::endl; }
};

class B {
 public:
  void Print() {}
  ~B() { std::cout << "~B" << std::endl; }
};

class C {
 public:
  void Print() {}
  ~C() { std::cout << "~C" << std::endl; }
};

void FunThrowExcption(bool th) {
  A a;
  a.Print();
  if (th) {
    throw TestExcetpion{};
  }
  B b;
  b.Print();
}

TEST(ExceptionTest, StackUnwinding) {
  try {
    C c;
    c.Print();
    FunThrowExcption(true);
  } catch (TestExcetpion e) {
    std::cout << "handle the exception" << std::endl;
  }
}

// 下面的代码，如果不是写在EXPECT_ANY_THROW里，抛出异常后，是不会进行栈展开的
TEST(ExceptionTest, NoCatch) {
  C c;
  c.Print();
  EXPECT_ANY_THROW(FunThrowExcption(true));
}

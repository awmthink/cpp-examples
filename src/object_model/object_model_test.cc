#include <gtest/gtest.h>

#include <iostream>

class BaseA {
 public:
  virtual void FuncA() { std::cout << "BaseA: FuncA()" << std::endl; }
  virtual ~BaseA() = default;
};

class BaseB {
 public:
  virtual void FuncB() { std::cout << "BaseA: FuncB()" << std::endl; }
  void PrintBaseBThis() { std::cout << this << std::endl; }
  virtual ~BaseB() = default;
};

class Derived : public BaseA, public BaseB {
 public:
  void FuncA() override { std::cout << "Derived: FuncA()" << std::endl; }
  void FuncB() override { std::cout << "Derived: FuncB()" << std::endl; }
};

void PrintPointer(const BaseA* p) { std::cout << p << std::endl; }
void PrintPointer(const BaseB* p) { std::cout << p << std::endl; }

TEST(ObjectModelTest, MultiClassDerviedTest) {
  auto* d = new Derived();
  std::cout << "drived pointer: " << d << std::endl;

  BaseA* pa = d;
  std::cout << "baseA pointer: ";
  PrintPointer(pa);
  // 这一步的指针向上转型时，并不是指针的简单拷贝，而是会进行一定的偏移调整
  BaseB* pb = d;
  std::cout << "baseB pointer: ";
  PrintPointer(pb);

  delete d;
}

TEST(ObjectModelTest, MultiClassDerviedRefTest) {
  Derived d = Derived();
  std::cout << "drived pointer: " << &d << std::endl;

  BaseA& pa = d;
  std::cout << "baseA pointer: ";
  PrintPointer(&pa);

  BaseB& pb = d;
  std::cout << "baseB pointer: ";
  PrintPointer(&pb);
}

TEST(ObjectModelTest, MultiClassDerviedThisPointer) {
  Derived d = Derived();
  std::cout << "drived pointer: " << &d << std::endl;
  // 调用从基类继承来的方法，获取到的this指针是基类的对象的地址
  std::cout << "address of this from BaseB function: ";
  d.PrintBaseBThis();
}

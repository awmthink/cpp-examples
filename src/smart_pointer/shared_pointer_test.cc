#include "smart_pointer/shared_pointer.h"

#include <gtest/gtest.h>

struct Foo {
  int a = 42;
};

struct Bar : public Foo {};

TEST(SmartPointerTest, SharedPointer) {
  SharedPointer<Foo> sp(new Foo);
  std::cout << sp->a << std::endl;

  std::cout << "ref count: " << sp.RefCount() << std::endl;

  SharedPointer<Foo> sp1(sp);  // 拷贝构造
  std::cout << "ref count: " << sp.RefCount() << std::endl;
  std::cout << "sp1 ref count: " << sp1.RefCount() << std::endl;

  SharedPointer<Foo> sp2;
  sp2 = sp1;  // 先拷贝构造临时对象，拷贝赋值
  std::cout << "sp ref count: " << sp.RefCount() << std::endl;
  std::cout << "sp1 ref count: " << sp1.RefCount() << std::endl;
  std::cout << "sp2 ref count: " << sp2.RefCount() << std::endl;

  SharedPointer<Bar> sp3(new Bar);
  SharedPointer<Foo> sp4;
  sp4 = sp3;  // 先调用的模板的拷贝构造为临时对象，再调用的移动赋值
}

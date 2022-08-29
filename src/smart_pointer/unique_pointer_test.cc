#include "smart_pointer/unique_pointer.h"

#include <gtest/gtest.h>

struct Foo {
  int a_ = 42;
};

struct Bar : public Foo {};

TEST(SmartPointerTest, UniquePointer) {
  UniquePointer<Foo> sp(new Foo);  // 指针构造函数
  std::cout << sp->a_ << std::endl;
  // UniquePointer<Foo> sp4(sp);
  // //报错！在定义了移动构造函数时，默认的拷贝构造不会自动生成
  UniquePointer<Foo> sp1(std::move(sp));  // 移动构造

  UniquePointer<Bar> sp2(new Bar);  // 指针构造函数
  UniquePointer<Foo> sp3;           // 空指针构造函数
  sp3 = std::move(sp2);             // 移动构造+移动赋值
}

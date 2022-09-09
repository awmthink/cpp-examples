#include <gtest/gtest.h>

#include <iostream>

int global_int;

static float static_pi = 3.14;

class Foo {
 public:
  static int static_in_class;
};

int Foo::static_in_class = 0;

TEST(StorageTest, PrintAddress) {
  float stack_object = .0F;
  int *heap_object = new int{0};

  constexpr int local_const = 42;

  std::cout << "stack object: " << &stack_object << std::endl;
  std::cout << "heap object: " << heap_object << std::endl;
  std::cout << "global object: " << &global_int << std::endl;
  std::cout << "global static object: " << &static_pi << std::endl;

  std::cout << "static object in class: " << &Foo::static_in_class << std::endl;

  std::cout << "local const " << &local_const << std::endl;

  std::cout << "c string literal " << &"hello,world" << std::endl;

  delete heap_object;
}

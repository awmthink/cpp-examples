#include "new_delete/heap_tracker.h"

#include <gtest/gtest.h>

TEST(NewDeleteTest, HeapTrackerTest) {
  class Bar {
    int a = 0;
    bool b = true;

   public:
    virtual void f() {}
  };
  // 这里写多重继承，是为了验证基类this指针了派生类的地址不同的问题
  class Foo : public Bar, public HeapTracker {
   public:
    void f() override {}
  };
  auto *f = new Foo();
  std::cout << "derived ptr: " << f << std::endl;
  std::cout << std::boolalpha << f->IsHeapBased() << std::endl;
  delete f;
}

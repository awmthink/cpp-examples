#include "new_delete/heap_tracker.h"

#include <gtest/gtest.h>

TEST(NewDeleteTest, HeapTrackerTest) {
  class Bar {
    int a_ = 0;
    bool b_ = true;

   public:
    virtual void Fun() { std::cout << a_ << b_ << std::endl; }
  };
  // 这里写多重继承，是为了验证基类this指针了派生类的地址不同的问题
  class Foo : public Bar, public HeapTracker {
   public:
    void Fun() override {}
  };
  auto *f = new Foo();
  std::cout << "derived ptr: " << f << std::endl;
  std::cout << std::boolalpha << f->IsHeapBased() << std::endl;
  delete f;
}

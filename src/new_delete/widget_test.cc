#include "new_delete/widget.h"

#include <gtest/gtest.h>

TEST(NewDeleteTest, WidgetTest) {
  Widget *w1 = new (__FILE__, __LINE__) Widget{1};
  delete w1;
  Widget *w2 = nullptr;
  try {
    w2 = new (__FILE__, __LINE__) Widget{0};
  } catch (...) {
    std::cout << "catch exception" << std::endl;
  }
  delete w2;

  // 由于未重载全局的nothrow版本，所以会出现申请和释放不一致的问题，导致asan报错
  // int *arr = new (std::nothrow) int{0};
  int *arr = int{0};
  delete arr;
}
#include <gtest/gtest.h>

#include <any>

TEST(VocabularyTests, AnyUsage) {
  std::any a;
  EXPECT_TRUE(!a.has_value());

  a = 1;
  EXPECT_TRUE(a.has_value());
  std::cout << "type: " << a.type().name()
            << ", value: " << std::any_cast<int>(a) << std::endl;

  EXPECT_THROW(std::any_cast<double>(a), std::bad_any_cast);

  a.reset();
  EXPECT_TRUE(!a.has_value());

  a = std::string("hello, world");
  auto *p = std::any_cast<std::string>(&a);
  std::cout << p->size() << std::endl;
}

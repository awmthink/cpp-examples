#include <gtest/gtest.h>

#include <variant>

TEST(VocabularyTests, VariantTest) {
  std::variant<std::string, int, char> obj;
  std::cout << obj.index() << std::endl;  // 0
  obj = 42;
  std::cout << obj.index() << std::endl;  // 1
  obj = 'A';
  std::cout << obj.index() << std::endl;  // 2
  obj = "Hello";
  std::cout << obj.index() << std::endl;  // 0
  EXPECT_THROW(std::get<int>(obj), std::bad_variant_access);
}

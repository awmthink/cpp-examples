#include <gtest/gtest.h>

#include <array>

class Container {
 public:
  void Reserve(std::size_t) {}  // NOLINT
};

class Container1 {
 public:
  void Resize(std::size_t) {}  // NOLINT
};

class Container2 {
 public:
  void Reserve(int) {}  // NOLINT
};

template <typename T>
struct HasReserve {
  struct Good {
    char dummy_;
  };
  struct Bad {
    std::array<char, 2> dummy_;
  };

  template <class U, void (U::*)(std::size_t)>
  struct SFINAE {};

  template <class U>
  static Good Reserve(SFINAE<U, &U::Reserve>*);

  template <class U>
  static Bad Reserve(...);

  static constexpr bool VALUE = sizeof(Reserve<T>(nullptr)) == sizeof(Good);
};

TEST(TemplateTest, SFINAETest) {
  // 这里如果使用的是const而不是constexpr，则会导致has_reserve<Container>::VALUE链接找不到符号
  EXPECT_TRUE(HasReserve<Container>::VALUE);
  EXPECT_FALSE(HasReserve<Container1>::VALUE);
  EXPECT_FALSE(HasReserve<Container2>::VALUE);
}

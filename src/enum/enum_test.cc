#include <gtest/gtest.h>

#include <iostream>

TEST(EnumTest, TwoTypeEnumeration) {
  // Color和Stoplight都是不限定作用域的枚举
  // 这就会导致Stoplight中存在重定义的问题，导致编译错误
  enum Color { red, yellow, green };
  // enum Stoplight {red,yellow,green};

  // Peppers是限定作用域的枚举类型
  // enum class等价于enum struct
  enum class Peppers { red, yellow, green };

  // 只能使用同类型的对象或枚举值来进行初始化或赋值
  Color c = yellow;
  Peppers p = Peppers::yellow;
  std::cout << "c: " << c << std::endl;
  std::cout << "p: " << static_cast<int>(p) << std::endl;

  int i = Color::red;  // 不限制作用域的枚举值可以作为整数
  // int j = Peppers::red; // 不支持类型转换
  EXPECT_EQ(0, i);
}

TEST(EnumTest, AsignEnumerationSize) {
  enum Color : long long { red, yellow, green };
  enum class Peppers : short { red, yellow, green };
  EXPECT_EQ(sizeof(long long), sizeof(Color));
  EXPECT_EQ(sizeof(short), sizeof(Peppers));
}

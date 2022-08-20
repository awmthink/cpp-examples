# Enumeration

枚举类型（enumeration）使我们可以将一组整形常量组织在一起。和类一样，每个枚举类型定义了一种新的类型。枚举值属于字面值常量类型。

C++包括两种枚举：限定作用域的和不限制作用域的。C++11新标准引入了限定作用域的枚举类型（scoped enumeration）。

## 定义方式

```cpp
// 不限制作用域的定义方式
enum Color {red, yellow, green};
// 未命名的、不限定作用域的枚举类型
enum {triangle, square, rectangle, circle}；
// 限制作用域的枚举类型
enum class Stoplight {red, yellow, green}; // 等价于enum struct
```

## 指定enum的大小

尽管每个命名的enum类型都定义了唯一的类型，但实际上enum是由某种整数类型表示的。在C++11新标准中，我们可以在enum的名字后加上冒号以及我想在该enum中使用的类型：

```cpp
enum class IndexType : short {
    unkown = -1,
    normal = 0,
    fast = 1
};
```

如果我们没有指定enum的潜在类型，则默认下限定作用域的enum成员类型是int。对于不限定作用域的枚举值类型来说，其枚举成员不存在默认类型，我们只知道成员的潜在类型足够大，肯定能够容纳枚举值。

## 枚举值的前置声明

在C++11新标准中，我们可以提前声明enum。enum的前置声明，必须指定其成员的大小：

```cpp
enum Color: int;
enum class Stoplight; // 限定作用域的枚举类型可以使用默认的类型int
```

## 赋值与形参匹配

要想初始化一个enum对象，必须使用该enum类型的另一个对象，或者它的一个枚举成员，因此，即使某个整数值恰好与枚举成员值相等，它也不能为枚举类型对象赋值或者为函数enum实参使用。

尽管我们不能直接将整型值传给enum形参，但是可以将一个不限定作用域的枚举类型的对象或枚举成员传给整形形参。
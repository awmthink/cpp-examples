# 右值引用与移动语义

## 左值和右值

**左值**是有标识符，可以取地址的表达式，最常见的情况有：

* 变量、函数或数据成员的名字
* 返回左值引用的表示式，如`++x`, `x = 1`, `count << ""`
* 字符串字面值，如`"hello, world"`，这个是很特殊的，因为一般的字面值都是右值。

**纯右值**（prvalue）是没有标识符，不可取地址的表达式，一般也称为**临时对象**，常见的情况有：

* 返回非引用类型的表达式，如`x++`，`x + 1`，`make_shared<int>(42)`
* 除了字符串字面值之外的字面值，如`42`, `true`等。

我们在语境中说到左值，一般都是指地址，可以理解为存放值的容器/盒子，而右值就是可以放在盒子中的值。

`int a = 42`是指把`42`个这值放到`a`这个地址里，所以这里`a`是一个左值。`int b = a`; 这里是指：把`a`中的值拷贝到`b`这个地址里。所以这里`b`是一个左值，而把`a`作为一个右值用。

## 左值引用与右值引用

**左值引用**就是绑定在左值上的引用，是这个左值的一个别名。

```c++
int a = 42;
int &b = a;
```
在C++11之前，右值只能绑定在常量引用上。

```c++
const int &b = 42;
```

而不可能创建一个右值对象的普通引用，比如：

```c++
Obj &ref = Obj();
```

右边是个临时对象，是个右值。

禁止这么做的原因是，危险，持有了一个已经销毁的对象，如果对`ref`来进行操作，相当于还在操作一个已经回收的栈的地址。

C++11以后引入了**右值引用**的概念，它允许我们为一个右值对象创建引用，即右值引用。

```c++
Obj &&ref = Obj();
```

特别需要注意的是虽然`ref`被绑定了一个右值，是一个右值引用，但`ref`在后面的使用中，行为就完全像一个左值了，最明显的，它有了变量名（标识符）。

右值引用，导致右值对象的生命周期延长。

```c++
result process_shape(const shape& shape1, const shape& shape2) {
  puts("process_shape()");
  return result();
}
result&& r = process_shape(circle(), triangle());
```
上面的调用导致，`result()`这个临时对象，不会在这句语句执行完时，销毁，而是当`r`销毁时，才会最终销毁。但这个规则只对纯右值有效，如果是通过`std::move`转发出来的，`xrvalue`就不行了。

```c++
result&& r = std::move(process_shape( circle(), triangle())); 
```

`std::move`返回的的是一个`xvalue`，标准规定了，不能通过右值引用来延长`xvalue`的生命周期。

上面的这个用法这个是不行的，这句执行完时，result的析构将会调用，`r`引用的是一个未定义的对象。

## 为什么要搞一个新的“右值引用”

主要是为了利用一些即将消亡的一些对象的数据，避免一些没必要的拷贝，提升性能。

```c++
string result = string("Hello, ") + name + ".";
```

上面这段代码的实现执行逻辑是：

1. 用"Hello"构造一个新的匿名字符串对象
2. 执行operator +(const string&, const string &) 返回一个新的临时对象，拷贝两个string中的内容。
3. 执行operator +(const string&, const char *) 再生成一个临时对象，将参数中的两个字符串的内容再拷贝一遍。
4. 构造生成result，在有编译器优化的情况下，可以和步骤3合在一起

好的方法如下：

```c++
string result = "Hello, ";
result += name;
result += ".";
```

后面的2步，都是直接在result的数据上直接追加。

有了C++11的右值引用后，就可以有更好的解决方案，还是原来的那种写法，但执行逻辑已经变了。

1. 调用`operator+(string&&, const string&)`，直接在临时对象1上面执行追加操作，并把结果移动到临时对象2；`name`复制1次。
2. 调用`operator+(string&&, const char*)`，直接在临时对象2上面执行追加操作，并把结果移动到`result`；`"."`复制1次。

## 如何实现一个支持移动的类

如何实现移动？要让你设计的对象支持移动的话，通常需要下面几步：

* 你的对象应该有分开的拷贝构造和移动构造函数（除非你只打算支持移动，不支持拷贝——如`unique_ptr`）。
* 你的对象应该有`swap`成员函数，支持和另外一个对象快速交换成员。
* 在你的对象的名空间下，应当有一个全局的`swap`函数，调用成员函数`swap`来实现交换。支持这种用法会方便别人（包括你自己在将来）在其他对象里包含你的对象，并快速实现它们的`swap`函数。
* 实现通用的`operator=`。

上面各个函数如果不抛异常的话，应当标为 noexcept。这对移动构造函数尤为重要。

## 在函数中直接返回对象

在C++11之前，返回一个本地对象意味着这个对象会被拷贝，除非编译器发现可以做返回值优化（named return value optimization，或NRVO），能把对象直接构造到调用者的栈上。从C++11开始，返回值优化仍可以发生，但在没有返回值优化的情况下，编译器将试图把本地对象移动出去，而不是拷贝出去。这一行为不需要程序员手工用`std::move`进行干预——使用 `std::move`对于移动行为没有帮助，反而会影响返回值优化。

## 引用坍缩问题

这个事情基本发生成一个类似于代理的场景下：我们最终调用是`foo`，但是很多时候，我们调用是`bar`，让`bar`里面去执行`foo`，`foo`在这里这有点像`python`的装饰器函数。

常用的场景：
* bar里会启动一个线程去执行foo
* bar在foo的执行前后，加了一些时间测量或日志打印。


```c++
class Shape {};
class Circle : public Shape {};

void Foo(const Shape &) { std::cout << "Foo(const Shape &)" << std::endl; }

void Foo(const Shape &&) { std::cout << "Foo(const Shape &&)" << std::endl; }

void Bar(const Shape &s) {
  std::cout << "Bar(const Shape &)" << std::endl;
  Foo(s);
}

void Bar(const Shape &&s) {
  std::cout << "Bar(const Shape &&)" << std::endl;
  Foo(s);
}

int main(int argc, char *argv[]) {
  Bar(Circle{});

  return 0;
}
```

在上面的代码里，`bar`来代理`foo`的功能。但很遗憾的，`bar`并没有很好的完成代理，我们把一个临时对象，传给给`bar`，希望使用的是右值引用参数的版本调用，虽然`bar`正确的识别，用了右值引用的版本，但当`bar`里调用`foo`时，确是使用了`foo`的左值引用的版本。

这里的原理就是对于`void Bar(const Shape &&s)`，`s`在`Bar`里实际是一个绑定了右值的右值引用，它本身是一个左值。要解决这个问题，也很简单，就是给`Foo`传递参数时，调用一下`std::move`。

```c++
void Bar(const Shape &&s) {
  std::cout << "Bar(const Shape &&)" << std::endl;
  Foo(std::move(s));
}
```
还要解决的一个问题是，bar这种代理性的函数，可能要写很多个，能不能做成一个模板呢？

```c++
template<typename T>
void Bar(T s) {
    Foo(T);
}
```
上面这样写是不行的，正确的写法就是要用完全转发：

```c++
template<typename T>
void Bar(T&& s) {
    Foo(std::forward<T>(s));
}
```

因为在`T`是模板参数时，`T&&`的作用主要是保持值类别进行转发，它有个名字就叫**转发引用**（forwarding reference）。因为既可以是左值引用，也可以是右值引用，它也曾经被叫做**万能引用**（universal reference）。

## 完美转发

```c++
template<typname T>
void fun(T&& x) {}
```

针对上面的模板参数的完美转发的形式，有如下的规则：

1. 如果`fun`的实参为一个`左值Tp`或`右值引用Tp&&`类型，则模板参数`T`的类形为`Tp&`，形参`x`的类型为`Tp& &&`，坍缩为`Tp&`。
2. 如果`fun`的实参为一个`右值`或`std::move()`返回类型——`xvalue`，则模板参数`T`的类形为`Tp`，形参`x`的类型也为`Tp&&`。
3. `cv-qualifications`会被保留下来，如果`fun`的实参为一个`const T&`类型，则模板参数`T`的类形为`const Tp&`，形参`x`的类型也为`const Tp&`。

```c++
template <typename T>
constexpr auto GetTypeName() {
  constexpr auto prefix = std::string_view{"[with T = "};
  constexpr auto suffix = "]";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};

  const auto start = function.find(prefix) + prefix.size();
  const auto end = function.find_last_of(suffix);
  const auto size = end - start;

  return function.substr(start, size);
}

template <typename T>
void PrintTypeName(T&& arg) {
  std::cout << "type(T): " << GetTypeName<T>() << ", type(arg)："
            << GetTypeName<decltype(arg)>() << std::endl;
}

int main(int argc, char* argv[]) {
  int lvalue = 0;
  PrintTypeName(lvalue);  // int&,int&
  PrintTypeName(rvalueref);        // int&,int&
  PrintTypeName(3.14F);            // float, float&&
  PrintTypeName(std::string{""});  // std::__cxx11::basic_string<char>,
                                   // std::__cxx11::basic_string<char>&&
  const int ci = 1;
  PrintTypeName(ci);                 // const int&, const int&
  PrintTypeName(std::move(lvalue));  // int,int&&

  return 0;
}
```

值得注意的就是`右值`和`右值引用`是两回事，`int &&r = 0`，其中的`r`的类型是`右值引用`，但当我们把`r`传入函数中时，它是一个左值，也就是说右值引用的值类型是左值。`std::move()`虽然返回类型是一个右值引用，标准规定它的值类型是一个`xvalue`，也就是将亡值，实际是当成右值来用，可以直接调用接受右值引用类型的函数中。

```c++
void Fun(int& i) { std::cout << "int &" << i << std::endl; }

void Fun(const int& i) { std::cout << "const int & " << i << std::endl; }

void Fun(int&& i) { std::cout << "int && " << i << std::endl; }

int main(int argc, char* argv[]) {
  int i = 0;
  Fun(i);  // 匹配 int&

  int& ii = i;
  Fun(i);  // 匹配 int&，因为左值引用，就可以看成原对象，只是个别名

  const int j = i;
  Fun(j);  // 匹配 const int&

  int&& rref = 0;
  Fun(i);             // 匹配 int&，因为右值引用的值类型是左值
  Fun(0);             // 匹配 int&&
  Fun(std::move(i));  // 匹配 int&&

  return 0;
}
```

万能引用生效的地方只限定在以下两种情况下：

```c++
template <typename T>
void f(T&& x);

auto&& x = y;
```

下面的两种情况下都不满足：

```c++
template <typename T>
class Widget {
  void fun(T&& x);
};

template <typename T>
void f(std::vector<T>&& x);
```

## `std::forward`

现在我们来看最前的提到的代理函数`Bar`中使用到的`std::forward`的作用，如果不用`std::forward`，那么很明显，当传给`Bar`一个右值时，`s`的类值是右值引用，这个时候把一个右值引用入到了`Foo`中时，就只能匹配左值的类型了。

```c++
template <typename T>
void Bar(T&& s) {
  Foo(std::forward<T>(s));
}
```
而对于`std::forward<T>(arg)`它实际做的也就只是类型转换的事情：

1. 如果`T`是左值引用，则它返回一个左值
2. 如果`T`不是一个引用，`arg`是一个右值引用，则它返回一个右值，在这种情况下，它等价于`std::move`。

一个可能的`std::invoke`的实现如下：

```c++
template <typename Func, typename... Args>
auto invoke(Func&& f, Args&&... args) {
  return (std::forward<Func>(f))(std::forward<Args>(args)...);
}
```


## 扩展阅读

* [C++中值类别](https://zh.cppreference.com/w/cpp/language/value_category)
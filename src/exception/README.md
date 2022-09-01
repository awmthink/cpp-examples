# Exception Handling and Exception Safety

## 函数如何处理执行过程中遇到的问题

1. 直接终止运行，并以日志的形式报告错误。

```c++
void strcpy(char *dest, const char *source) {
  if (!dest || !source) {
    std::cerr << "Invalid arguments for strcpy.\n";
    exit(1);
  }
  while (*source) {
    *dest++ = *source++;
  }
  *dest = '\0';
}
```

2. 返回错误

```c++
bool strcpy(char *dest, const char *source) {
  if (!dest || !source) {
    return false;
  }
  while (*source) {
    *dest++ = *source++;
  }
  *dest = '\0';
  return true;
}
```
3. 忽略错误

```c++
void strcpy(char *dest, const char *source) {
  if (!dest || !source) {
    return;
  }
  while (*source) {
    *dest++ = *source++;
  }
}
```

4. 使用断言

```c++
void strcpy(char *dest, const char *source) {
  assert(dest != nullptr);
  assert(source != nullptr);
  while (*source) {
    *dest++ = *source++;
  }
}
```

5. 抛出异常

```c++
void strcpy(char *dest, const char *source) {
  if (!dest || !source)
    throw std::invalid_argument("Null pointers passed to strcpy.");
  while (*source)
    *dest++ = *source++;
  *dest = ’\0’;
}
```
到底应该使用哪一种方法？

> 这里涉及到了软件开发中的一个带有本质性的难题——错误处理。历来错误处理一直是软件开发者所面临的最大困难之一。Bjarne Stroustrup在谈到其原因时说道，能够探察错误的一方不知道如何处理错误，知道如何处理错误的一方没有能力探察错误，而直接采用防御性代码来解决，会使得程序的正常结构被打乱，从而带来更多的错误。这种困境是非常难以应对的——费心耗力而未必有回报。因此，更多的人采用鸵鸟战术，对可能发生的错误视而不见，任其自然。<br> C++、Java和其他语言对错误处理问题的回答是异常机制。这种机制在正常的程序执行流之外开辟了专门的信道，专门用来在不同程序模块之间报告错误，解决上述错误探察与处理策略分散的矛盾。然而，有了异常处理机制后，开发者开始有一种倾向，就是使用异常来处理所有的错误。我曾经就这个问题在comp.lang.c++.moderated上展开讨论，结果是发现有相当多的人，包括Boost开发组里的很多专家，都认为异常是错误处理的通用解决方案。

## 使用错误码的问题

C和golag中都使用错误码，使用错误码也是有优点也有缺点。

**缺点**：

1. 几乎每次的函数调用，都需要加一个if语句，对返回值进行检查。
2. c++的构造函数没有返回值，所以一般类的实际构造都不能在构造函数中进行，都需要提供一个可以返回错误码的Init的函数。
3. 如果调用层次很深的时候，就需要逐层检查错误码。
4. 同时因为错误信息只有错误码？当发生错误时，无法判断错误发生在哪一层？出错的位置和调用的位置可能位置很远。
错误容易被忽略掉，因为C/C++没有强制要求用户检查函数调用的返回值（甚至可以直接忽略），所以实际很容易在错误传递的过程中错误被丢弃了。

不过第4条可以通过扩展错误码的定义来规避，比如把`errorCode`定义为一个包括`int`和`string`的结构化，`string`用于说明错误发生的位置与具体原因。

**优点**：代码直白、开销低。

## 标准库中定义的异常

基类是`exception`，定义在`<exception>`头文件中。

- `bad_alloc`：定义在`<new>`头文件中，用于在分配内存失败时抛出异常。
- `logic_error`：定义在`<stdexception>`头文件中，它还进一步派生了：`length_error`、`domain_error`、`out_of_range`、`invalid_argument`等。
- `runtime_error`：定义在`<stdexception>`头文件中，它还进一步派生了：`range_error`、`overflow_error`、`underflow_error`等。
- `bad_cast`：定义在`<type_info>`头文件中，用于在使用`dynamic_cast`对引用类型进行转换失败时。

另外其他的一些标准库组件可能也定义了一些异常类，比如：`chrono`中定义了`nonexistent_local_time`和`ambiguous_local_time`。

## 栈展开（Stack Unwinding）

```c++
void fun(int n) {
    int x = 42;
    int *p = new int[n];
    // ...
}
int main() {
    try{
        int size = 1000;
        func(size);
    } catch(const std::bad_alloc &e) {
        // deal with shortage of memory here..
    }
    // ...
}
```
当fun函数执行过程中，如果内存不足，那么在使用`new`来分配存储时，就会招出异常，这时，程序并不是直接退出，也不是直接跳到了`catch`语句处进行执行，而是先进行`栈展开`。

1. `int *p = new int[n];`它内部是调用`Operator new[]`，在`operator new[]`函数中抛出异常：`std::bad_alloc`
2. 控制流从`operator new[]`函数中回到`fun`函数。
3. 销毁x
4. 销毁n
5. 控制流从`fun`回到`main`
6. 销毁size
7. 捕获异常
8. 处理异常

## `try-catch`使用上的注意事项

`catch`语句可以有多个，每个`catch`语句中捕获并处理一类异常，彼此之间可以有继承关系，当捕获异常时，总是从上到下找到一个符合的，而不是**最合适的**的`catch`分支来处理。

```c++
try {
  do_something();
} catch (const std::runtime_error &re) {
  // deal with runtime_error
} catch (const std::exception &e) {
  // deal with other kinds of exceptions
} catch (...) {
  // deal with other things
}
```

`try-catch`的结构一定是至少包括了2个作用域的，我们在`catch`的处理部分是无法访问`try`代码块中的对象的，这也是因为在异常处理时，因为需要进行栈展开，在执行`catch`语句块时，`try`语句块中的对象都已经销毁了。

如果我们没有使`try-catch`来捕获并处理异常，那么异常招出时，将直接会调用`std::terminate`来终止程序，这时候会不会执行栈展开，取决于编译器的实现。

如何捕获构造函数初始化列表中可能抛出的异常？

```c++
template <typename T>
class Array {
 public:
  Array(std::size_t n) try : size_(n), data_(new T[n]{}) {
  } catch (const std::bad_alloc &ba) {
    std::cerr << "No enough memory" << std::endl;
    throw;
  }
};
```

## 异常安全保证

当我们在说一个函数是异常安全的时候，一般有3个层次：

1. **Nothrow guarantee**：函数使用`nothrow`承诺不抛出异常
2. **Strong guarantee**：承诺当异常发生时，整个程序的状态未发生改变，保持和抛出异常前一致，就好像函数未被调用过一样。
3. **Weak guarantee**：承诺如果一个异常被抛出时，程序中的所有对象保持一个有效的状态：1）没有破损的对象；2）保持一致性。

> Effective C++ Item 29: A software system is either exception-safe or it’s not. There’s no such thing as a partially exception- safe system. If a system has even a single function that’s not exception-safe, the system as a whole is not exception-safe. <br>  A function can usually offer a guarantee no stronger than the weakest guarantee of the functions it calls.

## 异常安全函数示例

示例一：非异常安全的函数，因为new分配内存时，可能发生异常，这时m_data已经被释放了，本对象当前处理一个损坏的状态。

```c++
class Array {
  int *m_data;
  std::size_t m_size;

 public:
  Array &operator=(const Array &other) {
    if (this != &other) {
      delete[] m_data;
      m_data = new int[other.m_size];
      std::copy(other.m_data, other.m_data + other.m_size, m_data);
      m_size = other.m_size;
    }
    return *this;
  }
};
```

示例二：强异常安全函数，中间的new可能会抛出异常，但不会有任何影响。

```c++
class Array {
 public:
  Array &operator=(const Array &other) {
    auto new_data = new int[other.m_size];
    std::copy(other.m_data, other.m_data + other.m_size, new_data);
    delete[] m_data;
    m_data = new_data;
    m_size = other.m_size;
    return *this;
  }
};
```

示例三：强异常安全，其中的`Array(other)`进行拷贝构造是有可能发生异常的，但即使发生的异常，对程序当前状态不会有任何改变

```c++
class Array {
 public:
  void swap(Array &other) noexcept {
    using std::swap;
    swap(m_size, other.m_size);
    swap(m_data, other.m_data);
  }
  Array &operator=(const Array &other) {
    Array(other).swap(*this);
    return *this;
  }
};
```

示例四：把上面的例子中的`Array`变成了一个类模板，这时候情况就变了，因为`T`我们不清楚具体是什么类型，它的拷贝函数是否是异常安全的。
所以下面的代码中，如果`copy`执行数据拷贝时，发生了异常，那么由于给`m_data`分配了存储，所以会导致内存泄漏。

```c++
template <typename T>
class Array {
 public:
  Array(const Array &other)
      : m_data(new T[other.m_size]), m_size(other.m_size) {
    std::copy(other.m_data, other.m_data + other.m_size, m_data);
  }
};
```
我们可以进行异常处理，来把上面的函数变为异常安全的：

```c++
template <typename T>
class Array {
 public:
  Array(const Array &other)
      : m_data(new T[other.m_size]), m_size(other.m_size) {
    try {
      std::copy(other.m_data, other.m_data + other.m_size, m_data);
    } catch (...) {
      delete[] m_data;
      throw;
    }
  }
};
```
那么显然，对应的拷贝赋值运算符也不是异常安全的：

```c++
template <typename T>
class Array {
 public:
  Array &operator=(const Array &other) {
    auto new_data = new T[other.m_size];
    std::copy(other.m_data, other.m_data + other.m_size, new_data);
    delete[] m_data;
    m_data = new_data;
    m_size = other.m_size;
    return *this;
  }
};
```
我们也可以加上异常处理来让上面的函数变为异常安全的，但更好的做法是，使用拷贝构造结合`swap`函数。

```c++
template <typename T>
class Array {
 public:
  void swap(Array &other) {
    using std::swap;
    swap(m_size, other.m_size);
    swap(m_data, other.m_data);
  }
  Array &operator=(const Array &other) {
    Array(other).swap(*this);
    return *this;
  }
};
```



## Google C++ Style里禁用异常的原因

Google C++ Style文档里的原文：

> 鉴于 Google的现有代码不能承受异常，使用异常的代价要比在全新的项目中使用异常大一些。转换(代码来使用异常的)过程会缓慢而容易出错。我们不认为可代替异常的方法，如错误码或断言，会带来明显的负担。我们反对异常的建议并非出于哲学或道德的立场，而是出于实际考虑。因为我们希望在Google使用我们的开源项目，而如果这些项目使用异常的话就会对我们的使用带来困难，我们也需要反对在Google的开源项目中使用异常。如果我们从头再来一次的话，事情可能就会不一样了。

Reddit上的侧面论据：

> 我过去在Google工作，写了风格指南初稿的Craig Silverstein说过他对禁用异常感到遗憾，但他当时别无选择。在他写风格指南的时候，不仅他们使用的编译器在异常上工作得很糟糕，而且他们已经有了一大堆异常不安全的代码了。


## 其他禁用异常的原因

一些游戏项目为了追求高性能，也禁用异常。这个实际上也有一定的历史原因，因为今天的主流C++编译器，在异常关闭和开启时应该已经能够产生性能差不多的代码（在异常未抛出时）。代价是产生的二进制文件大小的增加，因为异常产生的位置决定了需要如何做栈展开，这些数据需要存储在表里。典型情况，使用异常和不使用异常比，二进制文件大小会有约百分之十到二十的上升。

```c++
#include <vector>

int main(int argc, char *argv[]) {
  std::vector<int> v{1, 2, 3, 4, 5};
  v.push_back(20);

  return 0;
}
// g++ -o test -fno-exceptions ../playground/main.cc  -std=c++17
```

上面的代码在本机上测试，不加-fno-exception时，编译出来的可执行文件为24k，关闭了异常后，大小为23k，相差不到0.5%。

## 异常的其他问题

对它的主要批评有2个：

1. 异常违反了“你不用就不需要付出代价”的 C++ 原则。只要开启了异常，即使不使用异常你编译出的二进制代码通常也会膨胀。
2. 异常比较隐蔽，不容易看出来哪些地方会发生异常和发生什么异常。

对于第1点，所谓的开销就是上面说的性能与二进制文件大小的问题，首先性能不是个问题，在不发生异常的情况下，现在性能相差无几；但二进制文件大小问题，对现代程序开发来说，实际关系不大。

对于第2点，确实如此，C++里没有强制的约束，让开发人员，在开函数或类方法时，显式的标记当前调用过程是否可能会抛出异常。从 C++17 开始，C++甚至完全禁止了以往的动态异常规约，你不再能在函数声明里写你可能会抛出某某异常。你唯一能声明的，就是某函数不会抛出异常——`noexcept`、`noexcept(true/false)`。这也是C++的运行时唯一会检查的东西了。如果一个函数声明了不会抛出异常、结果却抛出了异常，C++ 运行时会调用 std::terminate 来终止应用程序。不管是程序员的声明，还是编译器的检查，都不会告诉你哪些函数会抛出哪些异常。

```c++
#include <cstdint>
#include <iostream>

int sum(int a, int b) noexcept {
  if (a == b) {
    throw std::runtime_error("a == b");
  }
  return a + b;
}

int main(int argc, char* argv[]) {
  try {
    sum(1, 1);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
```

上面的代码时，因为`sum`被标记为`noexcept`，所以`main`中的`try-catch`就会不起作用，程序会直接中止，打印异常相关的信息。

```
terminate called after throwing an instance of 'std::runtime_error'
what(): a == b
[1] 14768 abort ./playgroud
```

当然，不声明异常是有理由的。特别是在**泛型编程**的代码里，几乎不可能预知会发生些什么异常。我个人对避免异常带来的问题有几点建议：

* 写异常安全的代码，尤其在模板里。可能的话，提供强异常安全保证 ，在任何第三方代码发生异常的情况下，不改变对象的内容，也不产生任何资源泄漏。
* 如果你的代码可能抛出异常的话，在文档里明确声明可能发生的异常类型和发生条件。确保使用你的代码的人，能在不检查你的实现的情况，了解需要准备处理哪些异常。
* 对于肯定不会抛出异常的代码，将其标为`noexcept`。注意类的特殊成员（构造函数、析构函数、赋值函数等）会自动成为`noexcept`，如果它们调用的代码都是`noexcept`的话。所以，像`swap`这样的成员函数应当尽可能标成`noexcept`。

C++ 的标准容器在大部分情况下提供了强异常保证，即：一旦异常发生，现场会恢复到调用函数之前的状态，容器的内容不会发生改变，也没有任何资源泄漏。

## C++的零开销原则

零开销原则是C++设计原则，所说的是：

* 你无需为你所不用的付出。
* 你所用的正与你所能合理手写的效率相同。

总而言之，这表示不应该添加某特性到C++，如果该特性引发的开销，不论在时间还是空间方面，会大于程序员自己实现该特性产生的开销。语言中仅有的两个不遵循零开销原则的特性是**运行时类型鉴别**与**异常**，从而这是大多数编译器包含关闭它们的开关的原因。


## 扩展阅读

* [使用契约的思想来解决程序异常处理](https://blog.csdn.net/myan/article/details/1921)
* [C++FAQ: Excetpions and Error Handling](https://isocpp.org/wiki/faq/exceptions)
* [标准库中的所有异常类型](https://zh.cppreference.com/w/cpp/error/exception)
* [C++异常与代替品](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1947r0.pdf)

# Operator new 和 Placement new

> 版权声明：本文为CSDN博主「GKxx」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。<br>原文链接：https://blog.csdn.net/qq_39677783/article/details/124704501

## 全局的`operator new/delete`

当我们在C++中写出一条`new`表达式时，它的执行分为两步：
1. 分配所需的内存
2. 在分配的内存上构造对象

这两步中，C++允许我们控制的是第一步，第二步是我们无法控制的。默认情况下，如果用new创建单个对象，第一步会调用一个默认版本的名为`operator new`的函数；如果是用`new[]`创建“动态数组”，那么第一步调用的是默认版本的名为`operator new[]`的函数。它们的声明如下：

```c++
void *operator new(std::size_t size);
void *operator new[](std::size_t size);
```
`operator new`有二点特别的要求：

1. 不能是inline的
2. 必须是全局的，不能在任何namespace里

这里的参数`size`是编译器帮我们计算出的一个值。对于`new T(args)`来说，它调用`operator new(sizeof(T))`；对于`new T[n]`来说，它调用`operator new[](sizeof(T) * n)`。这两个函数并不管你要在这片内存上放什么类型的对象，`operator new[]`也并不知道你需要放多少个对象。它们的工作就是单纯分配一片内存。

同样地，当我们写出一条`delete`表达式时，它的执行也分为两步：

1. 析构对象
2. 释放内存

我们能控制的是第二步，第一步是无法控制的。默认情况下，如果使用`delete`，第二步会调用一个默认版本的名为`operator delete`的函数；如果使用`delete[]`，第二步会调用一个默认版本的名为`operator delete[]`的函数。它们的声明如下：

```c++
// operator delete/delete[] are not inlined
// not in any namespace
void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;
```
这两个函数的参数都是传给`delete`或`delete[]`的那个指针。注意，`delete`不应该抛出异常。

这几个函数在标准库中都有默认的版本，但如果你自己定义了，那么这并不会构成redefinition，而是会让编译器优先选择你自己定义的版本。乍一看，`operator new/new[]/delete/delete[]`与`malloc/free`没什么区别——连参数类型和返回值都一样，但首先你要知道的两个关键信息是：

1. C++ `new`必然会分配一定的内存，哪怕传给它的`size为`零。
2. 默认版本的`operator new/new[]`在内存不足时抛出`std::bad_alloc`异常。

因此，一个用`malloc`实现的`operator new`至少得长这样：（`operator new[]`同理，下同）

```c++
void *operator new(std::size_t size) {
  if (size == 0)
    ++size;
  auto ptr = std::malloc(size);
  if (ptr)
    return ptr;
  else
    throw std::bad_alloc{};
}
```

而不能只是简单地调用`malloc`了事。信奉实用主义的人可能会觉得：代码能跑不就行了？但是我们在C++中重载运算符的时候总是要牢记一条准则：让你的重载运算符的行为与内置行为尽可能保持一致，例如赋值运算符要返回引用、后置递增运算符要返回递增前的拷贝，等等。不然你就必须接受这一事实：你自己编写的这个运算符与你长期学习与实践中养成的某些习惯相违背，而这往往导致错误。

事实上，C++中的`operator new`还要比这更复杂一些。当它在遭遇内存不足的时候，它并不是直接抛出一个异常，而是想方设法地做一些调整，并且不断尝试重新分配，直至成功分配到内存或者确信无法分配内存为止。调整的方法是调用一个所谓的“new-handler”。一个使用`malloc`的`operator new`如下：

```c++
void *operator new(std::size size) {
    if (size == 0) {
        size = 1;
    }
    while(true) {
        auto ptr = std::malloc(size);
        if (ptr) {
            return ptr;
        }
        auto global_handler = std::get_new_handler();
        if (global_handler) {
            global_handler();
        } else {
             throw std::bad_alloc{};
        }
    }
}
```
如果这个`new-handler`不是空指针，就意味着我们还有救，那么我们调用这个函数来做一些适当的调整，这个函数可能会想办法释放掉一些内存来使得有更多的内存可以被使用，然后我们再次尝试分配内存，如此循环。但如果我们发现这个`new-handler`是空指针，那就意味着标准库`new-handler`对此也无能为力了，我们只得两手一摊，抛出`std::bad_alloc`异常。

`operator delete`则要简单很多，你唯一需要保证的就是`delete`一个空指针是无害的。例如，一个使用`free`实现的`operator delete`如下：

```c++
void operator delete(void *ptr) noexcept {
  std::free(ptr);
}
```

这在`ptr`为空指针的时候当然也是没问题的，因为`free`一个空指针什么都不会发生。

讲到这里，你应该已经明白`operator new/new[]/delete/delete[]`的重载与其它重载运算符的不同之处。如果我们显式地调用`::operator new(N)`，其实就是在分配`N bytes`的内存，和我们平常写的`new`表达式并不一样。

## 替换`operator new/delete`

我们何时需要替换这些函数呢？可能是出于这些原因：

1. 为了记录程序对于动态内存的使用，包括记录分配和释放来检测内存泄漏，或者做一些其它统计，比如程序在运行期间分配的内存的大小分布、寿命分布、最大动态分配量等等。
2. 为了提升效率。这些函数的默认版本采取的策略必须能够应付各种各样的内存配置需求，因此它们的效率虽然不会太差，但也并不在任何情况下有极佳的表现。因此假如你对你的程序的内存运用有着深刻的认识，你可能会发现自己定制的`operator new/delete`无论是时间上还是空间消耗上都胜过默认的版本。

替换全局的`operator new/new[]/delete/delete[]`是没有问题的，比方说当我想检查程序中的内存泄漏时，我可以在这几个函数中记录内存的分配与释放。但是注意，标准库的许多容器使用的默认`allocator`都会调用全局的`::operator new`，所以在全局范围重载这些函数相当于将标准库的朋友们全都邀请到你家来做客，你要确保你的这些函数能够应付它们的需求。

## 类别定制的版本

很多时候，我们可能只是想为我们自己定义的class提供特殊的内存配置方式，并不想惊扰其他人。这时我们可以为class提供一个`static`版本的`operator new/new[]/delete/delete[]`重载。例如：

```c++
class Widget {
 public:
  static void *operator new(std::size_t size);
  static void *operator new[](std::size_t size);
  static void operator delete(void *ptr) noexcept;
  static void operator delete[](void *ptr) noexcept;
}
```
当我们使用new来创建一个`Widget`类型的对象时，这个`Widget::operator new`将会成为比全局的`::operator new`更好的匹配；`delete`也是如此。

## Placement new/delete

除了这些只接受一个`size`参数的`operator new`和只接受一个`void *`参数的`operator delete`之外，C++还允许定义这些函数的带有额外参数的版本，称为“placement-new/delete”。标准库已经为我们定义好了两个著名的placement版本：

```c++
void *operator new(std::size_t size, const std::nothrow_t &) noexcept;
void *operator new[](std::size_t size, const std::nothrow_t &) noexcept;
void *operator new(std::size_t size, void *place) noexcept;
void *operator new[](std::size_t size, void *place) noexcept;

void operator delete(void *ptr, const std::nothrow_t &) noexcept;
void operator delete[](void *ptr, const std::nothrow_t &) noexcept;
void operator delete(void *ptr, void *place) noexcept;
void operator delete[](void *ptr, void *place) noexcept;
```

在new表达式中调用它们的方式是在new和类型名之间加上一对括号，在这对括号里传递额外的参数。标准库定义的这两种`placement new`分别是接受一个`const std::nothrow_t &`参数的和接受一个地址参数的版本，它们都保证不抛出异常。接受`const std::nothrow_t &`版本的`placement-new`类似于`malloc`，在内存不足的时候返回空指针，而非抛出异常，用法如下：

```c++
auto ptr = new (std::nothrow) T{Args};
```
事实上一直到1993年，C++中的`new`在内存不足时采取的策略一直是返回空指针，后来越来越多的人接受了抛出异常这一选项，于是C++用这种方式保留了不抛出异常、返回空指针的行为。`std::nothrow`是标准库定义的一个对象，其类型为`std::nothrow_t`，其实就是一个特殊的tag。这个`nothrow`版本的`placement-new`是允许用户自定义的，这时编译器将选用用户自定义的版本。

接受一个地址参数的`placement-new`版本，其意义是“在给定的地址构造对象”，因此它其实并不分配内存，而是假定内存已经在这个地址上分配完毕。因此这个函数什么都不做，直接把place返回出来。这个函数其实就是“placement-new”这个词的来源，同时也是许多人平常谈到“placement-new”的时候所指的函数。如果我们只是想在一个地址p上构造对象，就可以写：

```c++
new(ptr) T{Args};
```
注意，这个接受地址参数的placement-new不可以替换的。

此外，我们还可以自定义携带其它参数的placement new/delete，例如记录发出内存分配请求的行号、文件名。

```c++
void *operator new(std::size_t size, long line, const char *file) { /* ... */ }
auto ptr = new (__LINE__, __FILE__) Type(args);
```

我们知道，一条`new`表达式（当然可能是一条`placement-new`表达式）需要做两件事：首先调用对应的`operator new`函数分配内存，然后在所分配的内存地址上构造对象。构造对象的这一步需要调用一个构造函数，但这一步是有可能抛出异常的。假如构造函数抛出异常，对于普通的`new`表达式来说，它会调用普通的`operator delete`来释放刚刚分配的内存，以保证不出现内存泄漏；但是对于`placement-new`，由于它可能采取了特殊的内存分配方式，你必须还得提供一个特殊的`operator delete`来处理这一情况，这就是自定义`placement-delete`的用处所在。这个`placement-delete`所携带的额外参数与那个`placement-new的`额外参数相同。如果这个`placement-delete`不存在，那么运行期系统就无法知道究竟如何正确处理这个情况，于是什么也不做，连那个普通的`operator delete`也不会被调用，这就几乎肯定会导致内存泄漏，除非你的`placement-new`其实没有分配内存。

上面的规则好像只对自定义参数的`placement new/delete`生效，对于`nothrow`版本，则不生效。

## Sized-delete

用于在执行delete时，感知到释放的那块存储的大小。

在C++14之前，sized-delete只允许定义在类中，作为static成员函数。

```c++
struct Widget {
  int x;
  static void operator delete[](void *p, std::size_t sz) {
    std::cout << "size == " << sz << ’\n’;
    ::operator delete[](p);
  }
};
auto ptr = new Widget[100];
delete[] ptr;
```
如果一个类中定义了`sized-delete`没有定义普通`delete`则会调用`sized-delete`版本。

在C++14之后全局的sized-delete也是被允许定义的，但编译器可能选择`sized`的版本，也可能选择`unsized`的版本，标准库版本的sized-delete的实现就是直接调用了unsized的版本。

## Alignment-aware new/delete

C++17引用的新特性。

```c++
namespace std {
  enum class align_val_t : size_t {};
}
void *operator new(std::size_t size, std::align_val_t al);
void *operator new[](std::size_t size, std::align_val_t al);
void *operator new(std::size_t size, std::align_val_t al,
                   const std::nothrow_t &) noexcept;
void *operator new[](std::size_t size, std::align_val_t al,
                     const std::nothrow_t &) noexcept;
```
一般的`operator new`在申请内存时，都会按`__STDCPP_DEFAULT_NEW_ALIGNMENT__(16)`来对齐分配，如果某个对象通过`alignas`指定了特别的对齐要求，必须这个要求大于`__STDCPP_DEFAULT_NEW_ALIGNMENT__`，这时`alignment-aware`的版本就会被调用，将`alignof(T)`作为第二个参数传入`operator new`。

## Destroying-delete

C++20加入的新特性，一旦定义了detrorying-delete，那么执行delte时，就不会先执行析构函数，析构函数需要在detroying-delete里手动调用。

```c++
namespace std {
struct destroying_delete_t {
  explicit destroying_delete_t() = default;
};
inline constexpr destroying_delete_t destroying_delete{};  // a tag
}  // namespace std
struct T {
  void operator delete(T *ptr, std::destroying_delete_t);
  // Together with its sized and alignment-aware versions.
};
```

## 参考资料：

1. [cppreference: opeartor new](https://zh.cppreference.com/w/cpp/memory/new/operator_new)
2. [cppreference: opeartor delete](https://zh.cppreference.com/w/cpp/memory/new/operator_delete)

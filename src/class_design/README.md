# 自定义类型的设计

## 基于对象

基于对象研究的在用户自定义类型时所需要考虑的问题，包括了：

*   类的成员变量与成员函数声明与定义方式
*   类的访问控制
*   类的创建与销毁：构造函数与析构函数
*   类的拷贝控制函数：拷贝构造、拷贝赋值、移动拷贝、移动赋值
*   重载运算符
*   定义类型转换函数

## 数据抽象与封装

类的基本思想是数据抽象（data abstraction）和封装 (encapsulation）。数据抽象是一种依赖于接口 (interface）和实现(implementation）分离的编程（以及设计）技术。类的接口包括用户所能执行的操作；类的实现则包括类的数据成员、负责接口实现的函数体以及定义类所需的各种私有两数。

封装实现了类的接口和实现的分离。封装后的类隐藏了它的实现细节，也就是说，类的用户只能使用接口而无法访问实现部分。

类要想实现数据抽象和封装，需要首先定义一个抽象数据类型 (abstract data type )。在抽象数据类型中，由类的设计者负责考虑类的实现过程：使用该类的程序员则只需要抽象地思考类型做了什么，而无须了解类型的工作细节。

相比于C语言，C++ 语言提供了自定义类（class）的能力，class相比于C语言中的struct要强大的多。

1. 可以通过访问修饰符（access modifiers）控制成员的访问等级
2. 可以定义成员函数
3. 可以定义对象的创建与销毁过程

C++提供了三种类的成员访问控制符：

* `private`: 只能被类内部的代码或者友元访问
* `public`：任何人都可以访问
* `protected`：可以被类内部的代码、友元、类的继承类访问。

## `class`和`struct`

C++ 中，`class`和`struct`都可以用来定义类，它们的惟一区别是:

1. 默认的访问控制等级不同，`struct`是`public`，`class`是`private`
2. 当`struct`类作为子类继承一个其他类时，默认的继承访问控制是`public`

```cpp
struct Foo : Bar {}; // 等价于下面的声明
struct Foo : public Bar {};
```

## `this`指针

我们在类中可以使用`this`来表示指向这个类对象起始地址的指针，我们可以通过`this->mem`来访问类的成员或者通过`this->f()`来调用类的成员函数，但这看起来都没有什么必要。`this`指针一般的使用场景就是我们需要返回一个类对象自身的引用时：

```cpp
class Point2i {
  int x = 0;
  int y = 0;

 public:
  Point2i &SetX(int x) {
    this->x = x;
    return *this;
  }
  Point2i &SetY(int y) {
    this->y = y;
    return *this;
  }
};
Point2i p;
p.SetX(1).SetY(2);
```
`this`的另外一个使用场合时，在模板类的继承时，在派生类中访问父类的成员，需要使用`this`来访问。

```cpp
template <typename T>
class Matrix {
  T *data_;
};

template <typename T>
class Vector : public Matrix<T> {
 public:
  // 需要使用this->data_或者Matrix<T>::data_
  T& operator[](int i) { return this->data_[i]; } 
};
```

## 构造函数

### 对象的构造过程

类对象的构造过程：

1. 执行类的构造初始化列表，对类的成员进行初始化
2. 执行类成员的默认初始化，内置类型初始化为undefined value，对于类对象则调用其默认构建函数。
3. 执行构造函数函数体中的语句完成类对象的初始化工作。

类中成员的初始化过程，无论是在初始化列表中，还是默认的初始化，它的顺序都是遵循成员在类中定义的顺序。

```cpp
class Point2i {
  int x = 0;
  int y = 0;

 public:
  Point2i(int a, int b) : y(b), x(a) {}
};
```

上面的代码具有很强的误导性，容易让人以为`y`比`x`先初始化，实际上不是这样的。

强烈推荐使用使用类的初始化列表来进行成员的初始化，用于避免先执行默认初始化再执行赋值运算；同时一些特殊的成员也必须使用初始化列表，比如`const`成员，引用成员。

### 默认构造函数

如果我们没有显式的定义任何其他的构造函数，而且类中所有没有使用类内初始值（in-class initializer）的成员数据都是可以默认初始化的（default-initializable），那么编译器就会给我们合成一个默认的构造函数。

以下类型的成员是不满足可默认初始化的：

* 引用
* 没有类内初始化的const成员
* 一些类成员没有默认构造函数

### 委托构造

委托构造函数就是在我们一些重载的构造函数中，可以调用其他的构造函数。

```cpp
class Vector {
  size_t m_size = 0, m_capacity = 0;
  int *m_data = nullptr;

 public:
  // explicitly require a synthesized default ctor
  Vector() = default;
  Vector(size_t n) : m_size(n), m_capacity(n), m_data(new int[n]()) {}
  // delegate to Vector::Vector(size_t)
  Vector(int *begin, int *end) : Vector(end - begin) {
    for (int *p = m_data; begin != end; ++begin, ++p) *p = *begin;
  }
};
```
`Vector(int *begin, int *end)`构造函数则委托了`Vector(size_t n)`来完成了一些基本工作。

## 析构函数

析构函数是类中用于定义类对象销毁时行为的函数，它没有返回值，也就是`void`，可以直接省略了。析构函数前面都加了`~`标识符。析构函数不能被重载，它是惟一的。

对象析构时，成员的销毁顺序和构造函数正好相反。先执行析构函数体中定义的步骤，再去调用对应成员的析构函数来进一步释放成员。

析构函数是随着对象的生命周期结束自动调用的：

* 局部对象到达control flow的范围结束
* new出来的对象被delete
* 异常发生的时候

## const成员函数

对于没有对类的成员进行修改的成员函数，我们应该把它声明与定义为const的，对于`const`成员函数的调用，有如下的规则：

* 对于一个非const的对象，可以调用它的const成员函数和非const成员函数
* 对于一个const的对象，它只能调用const成员函数

对于同一个成员函数，定义为const和非const是可以形成重载的。

```cpp
class Vecotr {
  mutable int access_cnt;

 public:
  int &at(std::size_t n) {
    ++access_cnt;
    return data_[n];
  }
  const int &at(std::size_t n) const {
    ++access_cnt;
    return data_[n];
  }
```

**Bitwise const**：如果一个成员函数没有对它的任何成员数据进行修改，则称它为bitwise const的。

**Logical const**：如果一个成员函数可以保证用户不能通过这个函数来潜在的改变对象的状态，那我们称这个成员函数是logical const的，logical const的函数可能会个性对象的某些成员数据，但对于用户看到的对象看起来是没有变化的。

编译器只能够检查出一个成员函数是否是bitwise const的。bitwise-const虽然在成员函数内部保证了没有对对象的状态进行改变，但是返回了对象内部一些成员的广义引用，这就导致了在外部还是有可能通过这个返回的“引用”来改变这个对象。而logical-const是一种对类表现的对外状态不变性的保证。

```cpp
class Vecotr {
 public:
  int &at(std::size_t n) const { return data_[n]; }
};
```
上面的`at`函数虽然是bitwise-const的，但它返回了对象成员数据的一个非const的引用，用户可以通过这个引用来修改对象内部的数据。

## mutable修饰符

对于logical const的成员函数，可能会需要改成类的一些成员，但这些成员实际只是类内部可见，不对类的用户暴露的，我们可以使用mutable来达到目的。mutable一般用于修饰的成员，都是类的内部用于记录一些辅助信息的。

下面举2个mutable的应用的例子：

第一个例子是一个线程安全的队列，它的`Size`函数返回队列中元素的个数，应该是一个logical const的函数，但由于线程安全的保证，我们必须在获取内部元素个数时，对队列进行加锁，而锁则是对象内部的一个状态，我们要在const函数中修改对象，那这个锁应该就要是mutable的。

```cpp
class ThreadQueue {
 public:
  int32_t Size() const {
    std::locak_guard<std::mutex> lock(mx_);
    return queue_.size();
  }

 private:
  mutable std::mutex mx_ : std::queue<T> queue_;
};
```

第二个例子是`faiss`源代码中的一个例子，类中一些记录调用状态的成员被mutable修饰了。

```cpp
struct MockIndex : public faiss::Index {
  explicit MockIndex(idx_t d) : faiss::Index(d) { resetMock(); }
  mutable idx_t nCalled;
  mutable const float* xCalled;
  mutable idx_t kCalled;
  mutable float* distancesCalled;
  mutable idx_t* labelsCalled;
};
```

## 类的拷贝控制

我们把类的**拷贝构造函数**（copy constuctor）、**拷贝赋值运算符**（copy asssignment operator）、**移动构造函数**（move constructor）、**移动赋值运算符**（move asssignment operator）称为类的拷贝控制相关的函数。

### 拷贝构造函数

拷贝构造函数发生成对象初始化赋值或者函数按值传参数的过程中：

```cpp
A a1;
A a2 = a1; // 拷贝构造

void f(A a);
f(a2); // 拷贝构造
```

拷贝构造的形式为：

```cpp
A(const A &a) {}
```

它的参数类型必须为`const A&`，如果没有`const`，则不能使用常量对象来初始化，如果不使用引用类型，则变成了函数的值传递，那这个实参传递的过程中必须又要调用拷贝构造函数，陷入了死循环。

### 拷贝赋值运算符

发生的场景一般是对已经定义的对象间进行赋值。

```cpp
A a1, a2;
a1 = a2; // 拷贝赋值运算符
```

它的定义形式为：

```cpp
A& operator=(const A& a) {}
```

这里的返回的对象是引用类型，符合我们一般赋值表达式得到的是一个左值的习惯，这就意味着我们可以写出下面的语句：

```cpp
a1 = a2 = a3;
```

我们也可以显式的调用拷贝赋值运算符：

```
a1.operator=(a2);
```

### 移动构造函数

移动构造函数的一般定义形式为：

```cpp
A(A &&a) {}
```

### 移动赋值运算符

移动赋值运算符的一般定义形式为：

```cpp
A& operator=(A &&a) {}
```

### 拷贝赋值运算符的异常安全问题

如果我们定义的类中有动态分配过内存，或者管理了像文件句柄、网络连接这样的资源，在写类的拷贝赋值运算符时，要特别注意2个问题，一个是自我赋值的问题，另一个是异常安全的问题。

```cpp
Vecotr &Vector::operator=(const Vector &other) {
  if (this != other) {
    delete[] m_data;
    m_size = other.m_size;
    m_capcaity = other.m_capacity;
    m_data = new int[m_capacity];
    for (std::size_t i = 0; i < m_size; ++i) {
      m_data[i] = other.m_data[i];
    }
  }
  return *this;
}
```

上面的代码中虽然在`if`语句中处理了自我赋值的问题，但如果`new`表达式的地方抛出了异常，这时当前类的`m_data`已经被释放了，当前对象处于了一种无效的状态。

我们的处理方法也很简单，把内存分配写到内存释放之前，这样即使内存分配失败了，也没有破坏当前对象的状态。

```cpp
Vecotr &Vector::operator=(const Vector &other) {
  int *new_data = new int[other.m_capacity];
  for (std::size_t i = 0; i < other.m_size; ++i) {
    new_data[i] = other.m_data[i];
  }
  m_size = other.m_size;
  m_capcaity = other.m_capacity;
  delete[] m_data;
  m_data = new_data;

  return *this;
}
```

### 编译器合成规则

* 如果我们没有定义析构函数，则编译器会为我们自动合成，任何类不可能没有析构函数
* 如果我们没有定义默认构造函数，而且我没也没有定义其他任何构造函数，则编译器会尝试为我们自动合成
* 如果我们没有定义拷贝构造和拷贝赋值运算符，则编译器会尝试自动合成
* 如果我们定义了Big of Five中的任何一个，编译器就不会自动合成了（Since C++11）

### 什么时候需要自定义拷贝控制函数

什么样的类需要自定义拷贝控制函数？ 

除了管理 资源的类外，也有一些类，比如自动生成一`UUID`，每次拷贝或赋值时，是需要生成新的`UUID`的，而不是拷贝过来。

### 不能拷贝的对象

有一些类，它不适合拷贝，比如：`iostream`，那么我们可以把类的拷贝控制的函数定义为`=delete`的。

## 运算符重载

我们可以为我们自定义的类型重载运算符，以下的运算符，我们都可以重载：

```
+       -       *       /       %       ^
&       |       -       !       ,       =
<       >       <=      >=      ++      --
<<      >>      ==      !=      &&      ||
+=      -=      /=      %=      ^=      &=
|=      *=      <<=     >>=     []      ()
->      ->*     new     new[]   delete  delete[]
```

不能重载的运算符有：

```
::      .       .*      ?:
```

只能在自定义的类型上来重载运算符，不能对于内置类型进行重载运算符。在我们重载运算符时，不能改变运算符的运算**优先级**与**结合性**。

运算符重载的函数定义形式一般如下：

```cpp
A& operator+=(const A& other) {} // 在类内定义的

A operator+(const A& lhs, const A& rhs){} // 在类外定义的运算符重载函数
```

对于类内定义的重载运算符函数，它的第一个参数实际上是隐式的为`this`。

为什么不建议为`&&`、`||`、`,`这样的运算符重载，因为这样的运算符，它的求值顺序是固定的，比如`||`运算符，它一定是先求值左右，如果左边为假，才会求值右边的运算符。而对于我们自己重载的运算符来说，它是一个函数，函数的参数的求值顺序是未定义的。

什么时候将重载运算符定义为成员函数，什么时候定义为友元函数？

对于运算符左侧是类对象的，则可以定义为成员函数（函数的第一个函数是`this`）；如果操作符左侧不是类对象，则需要定义为友元函数，如输出运算符：`<<`。另外存在一种情况，即使是像`==`这样的运算符，两边是对称的类型，也存在一些区别，整体来说推荐定义为友元。


## 类型转换

### `bool`类型转换

对于`bool`类型转换函数，即使我们使用了`explicit`关键字来禁止隐式转换，但如果我们是在一些布尔上下文中，仍然会执行隐式的类型转换，布尔上下文包括了：

* 逻辑运算符中：`&&`、`||`、`!`
* 条件表达式中：`?:`
* 在一些控制语句中：`if`、`for`、`while`等。

```cpp
class Fraction {
  double numerator;
  double denominator;

 public:
  Fraction(double n, double d) : numerator(n), denominator(d) {}
  explicit operator bool() { return numerator == 0; }
};

int main(int argc, char* argv[]) {
  Fraction f(0, 1);
  // bool b = f; // error: cannot convert ‘Fraction’ to ‘bool’ in initialization
  if (f) {
    std::cout << "f is true" << std::endl;
  }

  return 0;
}
```

## 大括号初始化

大括号初始化分为二种，一种是`C++11`后推荐的，使用`{}`而不是`()`来进行对象的初始化构造，这样是为了避免容易把它看成一个函数的定义。`Matrix m{3,4}`等价于`Matrix m(3, 4)`；另一种是`std::initialize_list`，当我们在类中定义了：

```cpp
vector(std::initializer_list<T> l)
```

这样的构造函数时，我们可以这样创建一个`vector`

```cpp
vector<int> a = {1,2,3,4,5};
```

但这样就容易破坏我们说的大括号的第一种用法，比如：

```cpp
vector<int> a{10, 1};
```

如果按第一种用法，就是创建一个包括`10`个元素，每个元素初始化为`1`的数组。但由于第二种用法的存在，就变成了创建一个2个元素的数组。
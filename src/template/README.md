# 模板



## 变长模板

变长模板的基本定义形式如下：

```c++
template<class... Types>
struct Tuple {};

template <typename T, typename... Args>
void Print(T arg, Args... args) {
  std::cout << arg << ", ";
  if constexpr (sizeof...(args) > 0) {
    Print(args...);
  }
}
```
* 模板参数定义时使用的是`typename...`来声明一个变长参数的模板类型`Args`
* 模板函数的参数类型为`Args...`
* 我们可以使用`sizeof...`来在编译器获取参数的个数
* 使用`args...`来对不定长的参数进行展开传递
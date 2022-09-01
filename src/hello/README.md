# Hello, C/C++

## Language Standards

* Standards of C: C89/C90、C99、C11、C17、C23（comming soon）
* Standards of C++：C++98/03、C++11、C++14、C++17、C++20、C++23（comming soon），C++的标准目前是每三年会推出一个版本。

通过编译选项`-std=cx`或`-std=c++y`来指定使用的语言标准，比如：`-std=c17`、`-std=c++17`。我们在代码中可以通过一些编译器预定义的宏来检查我们的编译器使用的语言标准，在C代码中可以使用`__STDC_VERSION__`，在C++中可以使用`__cplusplus`。比如`201703L`就意味着使用的是C++17的标准。
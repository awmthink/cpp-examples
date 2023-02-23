#ifndef EXAMPLES_UNIQUE_POINTER_UNIQUE_POINTER_H_
#define EXAMPLES_UNIQUE_POINTER_UNIQUE_POINTER_H_

#include <utility>

// UniquePointer是一个对于标准库unique_ptr的模拟实现，
// UniquePointer是对于内存指针的一种RAII包装，利用析构函数来保证指针的释放。
// UniquePointer对持有的存储指针是独占的，不会和其他UniquePointer共享，
// 通过移动构造和移动赋值可以实现指针所有权的转移
template <class T>
class UniquePointer {
 public:
  // 通过祼指针来构造unique_ptr
  explicit UniquePointer(T *ptr = nullptr) : pointer_(ptr) {}

  // Release函数将释放对于指针的所有权，将内部指针返回给外部用户管理
  T *Release() noexcept {
    auto ptr = pointer_;
    pointer_ = nullptr;
    return ptr;
  }

  // 移动构造函数
  // 构造函数初始化列表，要放在noexcept后面
  UniquePointer(UniquePointer &&sp) noexcept : pointer_(sp.Release()) {}

  // 模板类型的构造函数，并不会被编译器认为是构造函数
  template <class U>
  UniquePointer(UniquePointer<U> &&sp) noexcept {
    pointer_ = sp.Release();
  }

  void Swap(UniquePointer &other) { std::swap(pointer_, other.pointer_); }

  T *Ptr() { return pointer_; }

  const T *Ptr() const { return pointer_; }

  // 注意这里的传参类型使用的是UniquePointer
  // 由于UniquePointer不支持拷贝构造，所以这个实现等价于
  // UniquePointer &operator=(UniquePointer&& sp) noexcept
  UniquePointer &operator=(UniquePointer sp) noexcept {
    sp.Swap(*this);
    return *this;
  }

  const T *operator->() const { return pointer_; }

  const T &operator*() const { return *pointer_; }

  T *operator->() { return pointer_; }

  T &operator*() { return *pointer_; }

  operator bool() {
    if (pointer_ == nullptr) {
      return false;
    }
    return true;
  }

  void Reset(T *ptr) { Swap(UniquePointer(ptr)); }

  ~UniquePointer() { delete pointer_; }

 private:
  T *pointer_ = nullptr;
};

#endif  // EXAMPLES_UNIQUE_POINTER_UNIQUE_POINTER_H_

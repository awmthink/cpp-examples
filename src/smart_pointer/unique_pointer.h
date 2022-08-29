#ifndef SRC_SMART_POINTER_UNIQUE_POINTER_H_
#define SRC_SMART_POINTER_UNIQUE_POINTER_H_

#include <iostream>
#include <utility>

template <class T>
class UniquePointer {
 public:
  template <class U>
  friend class UniquePointer;

  explicit UniquePointer(T *ptr = nullptr) : pointer_(ptr) {
    std::cout << "UniquePointer(T* ptr = nullptr)" << std::endl;
  }
  T *Release() {
    auto ptr = pointer_;
    pointer_ = nullptr;
    return ptr;
  }

  UniquePointer(UniquePointer &&sp) noexcept {
    std::cout << "UniquePointer(UniquePointer &&sp)" << std::endl;
    pointer_ = sp.Release();
  }

  template <class U>
  UniquePointer(UniquePointer<U> &&sp) noexcept {
    std::cout << "UniquePointer(UniquePointer<U> &&sp)" << std::endl;
    pointer_ = sp.Release();
  }

  void Swap(UniquePointer &other) { std::swap(pointer_, other.pointer_); }

  T *Ptr() const { return pointer_; }

  UniquePointer &operator=(UniquePointer sp) noexcept {
    std::cout << "operator=(UniquePointer sp)" << std::endl;
    sp.Swap(*this);
    return *this;
  }

  T *operator->() const { return pointer_; }

  T &operator*() const { return *pointer_; }

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

#endif  // SRC_SMART_POINTER_UNIQUE_POINTER_H_

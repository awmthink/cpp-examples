#ifndef EXAMPLES_SHARED_POINTER_SHARED_POINTER_H_
#define EXAMPLES_SHARED_POINTER_SHARED_POINTER_H_

#include <iostream>
#include <utility>

template <class T>
class SharedPointer {
 public:
  explicit SharedPointer(T *ptr = nullptr) : pointer_(ptr) {
    if (pointer_ != nullptr) {
      ref_count_ = new unsigned int{1};
    }
  }

  SharedPointer(const SharedPointer &sp) noexcept {
    if (sp.pointer_ == nullptr) {
      return;
    }
    pointer_ = sp.pointer_;
    ref_count_ = sp.ref_count_;
    *ref_count_ += 1;
  }

  template <class U>
  friend class SharedPointer;

  // 如果想要实现通过派生类的智能指针来创建基类的智能指针，则必须使用模板成员函数
  template <class U>
  SharedPointer(const SharedPointer<U> &sp) noexcept {
    if (sp.pointer_ == nullptr) {
      return;
    }
    pointer_ = sp.pointer_;
    ref_count_ = sp.ref_count_;
    *ref_count_ += 1;
  }

  SharedPointer(SharedPointer &&sp) noexcept { Swap(sp); }

  void Swap(SharedPointer &other) {
    std::swap(pointer_, other.pointer_);
    std::swap(ref_count_, other.ref_count_);
  }

  T *Ptr() { return pointer_; }

  const T *Ptr() const { return pointer_; }

  unsigned int RefCount() const {
    if (ref_count_ != nullptr) {
      return *ref_count_;
    }
    return 0;
  }

  SharedPointer &operator=(SharedPointer sp) noexcept {
    sp.Swap(*this);
    return *this;
  }

  const T *operator->() const { return pointer_; }

  const T &operator*() const { return *pointer_; }

  T *operator->() { return pointer_; }

  T &operator*() { return *pointer_; }

  SharedPointer &operator++() {
    pointer_++;
    return *this;
  }

  SharedPointer operator++(int) {
    SharedPointer tmp = *this;
    ++(*this);
    return tmp;
  }

  operator bool() const {
    if (pointer_ == nullptr) {
      return false;
    }
    return true;
  }

  void Reset(T *ptr) { Swap(SharedPointer(ptr)); }

  ~SharedPointer() {
    if (pointer_ == nullptr) {
      return;
    }
    *ref_count_ -= 1;
    if (*ref_count_ == 0) {
      delete pointer_;
      delete ref_count_;
      pointer_ = nullptr;
      ref_count_ = nullptr;
    }
  }

 private:
  T *pointer_ = nullptr;
  unsigned int *ref_count_ = nullptr;
};

#endif  // EXAMPLES_SHARED_POINTER_SHARED_POINTER_H_

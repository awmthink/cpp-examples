#ifndef SRC_NEW_DELETE_H_
#define SRC_NEW_DELETE_H_

#include <iostream>
#include <unordered_set>

class HeapTracker {
 public:
  using RawPtr = const void *;
  HeapTracker() = default;
  static void *operator new(std::size_t size) {
    void *ptr = ::operator new(size);
    memory_tracked_.insert(ptr);
    return ptr;
  }
  static void operator delete(void *ptr) {
    memory_tracked_.erase(ptr);
    ::operator delete(ptr);
  }

  bool IsHeapBased() const {
    // 如果通过基类指针获取一个子类对象的首地址？
    // 可以通过dynamic_cast这种用法
    // 基类的指针不是子类对象的首地址？ 因为可能存在多重继承的问题
    auto pos = memory_tracked_.find(dynamic_cast<RawPtr>(this));
    std::cout << "base ptr: " << this
              << ", derived ptr: " << dynamic_cast<RawPtr>(this) << std::endl;
    return pos != memory_tracked_.end();
  }

  virtual ~HeapTracker() = 0;

 private:
  static std::unordered_set<RawPtr> memory_tracked_;
};

HeapTracker::~HeapTracker() = default;

std::unordered_set<HeapTracker::RawPtr> HeapTracker::memory_tracked_{};

#endif  // SRC_NEW_DELETE_H_
#ifndef SRC_HEAP_TRACKER_HEAP_TRACKER_H_
#define SRC_HEAP_TRACKER_HEAP_TRACKER_H_

#include <iostream>
#include <unordered_set>
/*
 * \brief HeapTracker是一个用于跟踪对象是否是通过是在Heap上分配的跟踪器
 * 它通过类内重载operator new/delete的方式，接管了所以继承了HeapTracker
 * 对象的动态内存分配与释放。
 * HeapTracker将所有分配的地址记录在一个无序集合memory_tracked_中
 * 通过查询地址是否在memory_tracked_中来确认给定的对象是否为Heap上分配
 *
 * \note 这里特别里要处理的就是，在继承体系下，指向基类的指针地址，可能和该对象
 * 的实际地址不一致（多重继承下），这时候，如果要获取对象的真实地址，需要使用
 * dynamic_cast<const void*>来进行转换
 */
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
    // 如何通过基类指针获取一个子类对象的首地址？
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

#endif  // SRC_HEAP_TRACKER_HEAP_TRACKER_H_

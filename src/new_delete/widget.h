#ifndef SRC_NEW_DELETE_WIDGET_H_
#define SRC_NEW_DELETE_WIDGET_H_

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

class Widget {
 public:
  explicit Widget(int i) {
    if (i == 0) {
      throw std::invalid_argument("bad i");
    }
  }

  static void *operator new(std::size_t size) {
    std::cout << "[Widget] placement operator new" << std::endl;
    return ::operator new(size);
  }
  // sized-deallocation
  static void operator delete(void *ptr, std::size_t size) noexcept {
    std::cout << "[Widget] placement operator delete: " << size << std::endl;
    ::operator delete(ptr);
  }

  static void *operator new(std::size_t size, const std::string &file_name, uint64_t lineno) {
    std::cout << file_name << ":" << lineno << " [Widget] placement operator new(nothrow)"
              << std::endl;
    return ::operator new(size);
  }

  static void operator delete(void *ptr, const std::string &file_name, uint64_t lineno) noexcept {
    std::cout << file_name << ":" << lineno << " [Widget] placement operator delete(nothrow)"
              << std::endl;
    ::operator delete(ptr);
  }
};

#endif  // SRC_NEW_DELETE_WIDGET_H_

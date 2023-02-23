#include "shared_pointer.h"

int main() {
  SharedPointer<int> sp(new int{2});
  std::cout << *sp << std::endl;
}
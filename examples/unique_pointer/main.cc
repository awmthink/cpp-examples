#include "unique_pointer.h"

int main() {
  UniquePointer<int> up{new int{42}};
  UniquePointer<int> up1{std::move(up)};
}
#include <any>
#include <iostream>


int main() {
  std::any a;
  a = 1;
  std::cout << "type: " << a.type().name()
            << ", value: " << std::any_cast<int>(a) << std::endl;

  a.reset();

  a = std::string("hello, world");
  auto *p = std::any_cast<std::string>(&a);
  std::cout << p->size() << std::endl;
}
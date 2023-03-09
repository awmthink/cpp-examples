#include <iostream>
#include <string_view>

int main() {
  std::cout << std::string_view(__FILE__).substr(
                   std::string_view(__FILE__).find_last_of('/'))
            << std::endl;
}

#include <iostream>
#include <optional>

class Widget {
  int value_;

 public:
  explicit Widget(int v) : value_(v) {}
  void Func() const { std::cout << "Widget::value_ = " << value_ << std::endl; }
};

std::optional<Widget> Foo(int input) {
  if (input == 42) {
    return Widget{input};
  }
  return std::nullopt;
}

int main() {
  auto w = Foo(42);  // 42
  if (w) {
    w->Func();
  }

  auto w1 = Foo(1);
  auto r = w1.value_or(Widget{23});
  r.Func();  // 23
}
#ifndef SRC_CLASS_ORITEND_FRACTION_H_
#define SRC_CLASS_ORITEND_FRACTION_H_

class Fraction {
 public:
  explicit Fraction(int num, int den = 1)
      : numerator_(num), denominator_(den) {}

  explicit operator double() {
    return static_cast<double>(numerator_) / denominator_;
  }

 private:
  int numerator_;
  int denominator_;
};

#endif  // SRC_CLASS_ORITEND_FRACTION_H_

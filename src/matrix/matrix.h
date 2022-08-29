#ifndef SRC_MATRIX_MATRIX_H_
#define SRC_MATRIX_MATRIX_H_

#include <iostream>
#include <utility>

template <typename T>
class Matrix {
 public:
  Matrix() = default;

  Matrix(int r, int c) : rows_(r), cols_(c), data_(new T[r * c]()) {
    std::cout << "construrctor" << std::endl;
  }

  Matrix(int r, int c, T* ext_data)
      : rows_(r), cols_(c), data_(ext_data), own_data_(false) {}

  Matrix(const Matrix& m) {
    std::cout << "copy constructor" << std::endl;
    rows_ = m.rows_;
    cols_ = m.cols_;
    data_ = new T[rows_ * cols_];
    for (int i = 0; i < rows_ * cols_; i++) {
      data_[i] = m.data_[i];
    }
  }

  Matrix(Matrix&& m) noexcept {
    std::cout << "move constructor" << std::endl;
    this->Swap(m);
  }

  void Swap(Matrix& m) {
    std::swap(rows_, m.rows_);
    std::swap(cols_, m.cols_);
    std::swap(data_, m.data_);
    std::swap(own_data_, m.own_data_);
  }

  Matrix& operator=(Matrix m) noexcept {
    std::cout << "copy or move assignment operator" << std::endl;
    this->Swap(m);
    return *this;
  }

  Matrix operator-() const {
    Matrix neg(*this);
    for (int i = 0; i < rows_ * cols_; i++) {
      neg.data_[i] = -neg.data_[i];
    }
    return neg;
  }

  T& At(int i, int j) { return data_[i * cols_ + j]; }

  Matrix operator+(const Matrix& other) const {
    Matrix sum(rows_, cols_);
    for (int i = 0; i < rows_ * cols_; i++) {
      sum.data_[i] += data_[i] + other.data_[i];
    }
    return sum;
  }

  friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    for (int i = 0; i < m.rows_; i++) {
      for (int j = 0; j < m.cols_; j++) {
        os << m.data_[i * m.cols_ + j] << " ";
      }
      os << "\n";
    }
    os << std::flush;
    return os;
  }

  ~Matrix() {
    if (own_data_) {
      delete[] data_;
    }
  }

 private:
  int rows_ = 0;
  int cols_ = 0;
  T* data_ = nullptr;
  bool own_data_ = true;
};

#endif  // SRC_MATRIX_MATRIX_H_

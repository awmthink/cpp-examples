#ifndef SRC_MATRIX_MATRIX_H_
#define SRC_MATRIX_MATRIX_H_

#include <algorithm>
#include <iostream>
#include <utility>

template <typename T>
class Vector;

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
    Matrix neg(rows_, cols_);
    for (int i = 0; i < rows_ * cols_; i++) {
      neg.data_[i] = -data_[i];
    }
    return neg;
  }

  int Rows() const { return rows_; }

  int Cols() const { return cols_; }

  T& At(int i, int j) { return data_[i * cols_ + j]; }

  const T& At(int i, int j) const { return data_[i * cols_ + j]; }

  Matrix& operator+=(const Matrix& other) {
    for (int i = 0; i < rows_ * cols_; i++) {
      data_[i] += other.data_[i];
    }
    return *this;
  }

  Matrix& operator-=(const Matrix& other) {
    for (int i = 0; i < rows_ * cols_; i++) {
      data_[i] -= other.data_[i];
    }
    return *this;
  }

  Matrix operator+(const Matrix& other) const {
    Matrix sum(*this);
    return sum += other;
  }

  Matrix operator-(const Matrix& other) const {
    Matrix sum(*this);
    return sum -= other;
  }

  Vector<T> operator[](int i) { return Vector<T>{cols_, data_ + i * cols_}; }

  const Vector<T> operator[](int i) const {
    return Vector<T>{cols_, data_[i * cols_]};
  }

  void Reshape(int r, int c) {
    if (r == -1) {
      r = rows_ * cols_ / c;
    }
    if (c == -1) {
      c = rows_ * cols_ / r;
    }
    if (r * c != rows_ * cols_) {
      throw std::invalid_argument("r * c != rows_ * cols_");
    }
    rows_ = r;
    cols_ = c;
  }

  T* Data() { return data_; }

  const T* Data() const { return data_; }

  ~Matrix() {
    if (own_data_) {
      delete[] data_;
    }
  }

 protected:
  int rows_ = 0;
  int cols_ = 0;
  T* data_ = nullptr;
  bool own_data_ = true;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
  for (int i = 0; i < m.Rows(); i++) {
    for (int j = 0; j < m.Cols(); j++) {
      os << m.At(i, j) << " ";
    }
    os << (i == (m.Rows() - 1) ? '\0' : '\n');
  }
  return os;
}

template <typename T>
bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs) {
  if (lhs.Rows() != rhs.Rows() || lhs.Cols() != rhs.Cols()) {
    return false;
  }
  for (int i = 0; i < lhs.Rows(); i++) {
    for (int j = 0; j < lhs.Cols(); j++) {
      if (lhs.At(i, j) != rhs.At(i, j)) {
        return false;
      }
    }
  }
  return true;
}

template <typename T>
class Vector : public Matrix<T> {
 public:
  explicit Vector(int size) : Matrix<T>(1, size) {}

  Vector(int size, T* ext_data) : Matrix<T>(1, size, ext_data) {}

  Vector(const std::initializer_list<T>& il) : Vector(il.size()) {
    std::copy(il.begin(), il.end(), this->data_);
  }

  T& operator[](int i) { return this->data_[i]; }

  const T& operator[](int i) const { return this->data_[i]; }
};

#endif  // SRC_MATRIX_MATRIX_H_

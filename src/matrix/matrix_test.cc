#include "matrix/matrix.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

TEST(MatrixTest, Constructor) {
  Matrix<int> m(3, 4);
  std::cout << m << std::endl;
}

TEST(MatrixTest, CopyConstructor) {
  Matrix<int> m(3, 3);
  m.At(1, 1) = 3;
  Matrix<int> n = m;
  std::cout << n << std::endl;
  n.At(0, 0) = 1;
  m = n;
  std::cout << m << std::endl;
}

TEST(MatrixTest, VectorOfMatrix) {
  std::vector<Matrix<float>> matrix_vec;
  for (int i = 0; i < 5; i++) {
    // NOLINTNEXTLINE
    matrix_vec.emplace_back(3, 3);
  }
  std::cout << matrix_vec[3] << std::endl;
}

TEST(MatrixTest, MatrixAdd) {
  Matrix<int> m(3, 3);
  m.At(0, 0) = 1;
  m.At(1, 1) = 2;
  m.At(2, 2) = 3;

  Matrix<int> n(3, 3);
  n.At(0, 2) = 5;
  n.At(2, 0) = 5;

  Matrix<int> k = m + n;
  std::cout << k << std::endl;
}

TEST(MatrixTest, Getline) {
  std::istringstream iss("abc def\n xyzeidskd\n");
  std::string first_string;
  iss >> first_string;
  std::cout << first_string << std::endl;  // abc
  std::string l;
  std::getline(iss, l);
  std::cout << l << std::endl;  // <space>def
}

TEST(MatrixTest, NegativeMatrix) {
  Matrix<int> m{3, 3};
  m.At(1, 1) = 2;
  std::cout << -m << std::endl;
}

TEST(MatrixTest, VectorTest) {
  Vector<int> vi = {1, 2, 3, 4};
  std::cout << vi << std::endl;
  vi[2] = 5;
  vi.Reshape(-1, 1);
  std::cout << vi << std::endl;

  Matrix<int> m{3, 4};
  m[0][0] = 1;
  m[1][1] = 2;
  m[2][2] = 3;
  std::cout << m << std::endl;
}

#include <array>
#include <chrono>
#include <iostream>
#include <random>

// 版本0: 原始版本的矩阵乘法，效率差的原因是
// 内循环里对于B的访存是按列的，导致访存不连续，性能较差
template <int M, int N, int K>
void MatMulV0(const std::array<std::array<float, N>, M>& A,
              const std::array<std::array<float, K>, N>& B,
              std::array<std::array<float, K>, M>& C) {
  for (int i = 0; i < M; ++i) {
    for (int k = 0; k < K; ++k) {
      float v = 0;
      for (int j = 0; j < N; ++j) {
        v += A[i][j] * B[j][k];  // C[i][k] = A的第i行和B的第k列的内积
      }
      C[i][k] = v;
    }
  }
}

// 版本1: 变换内部2层的循环，使得A，B，C的访存都是行连续访问的
// 它的实现核心是，对于C矩阵，区分与版本0，它并没有完整的计算出来C[i,j]
// 而是计算C中的一行的部分结果（1/k），内层循环每完成一次，就累加一次
template <int M, int N, int K>
void MatMulV1(const std::array<std::array<float, N>, M>& A,
              const std::array<std::array<float, K>, N>& B,
              std::array<std::array<float, K>, M>& C) {
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      // c[i] = sum_j(A[i][j] * B[j])
      for (int k = 0; k < K; ++k) {
        C[i][k] += A[i][j] * B[j][k];
      }
    }
  }
}

// 版本3: 在版本0的基础上，使用寄存器缓存，来减少A和B从dram中的加载
// 核心思想是一次计算C中的一个tile
// 在版本0中，计算C中的一个tile需要从A和B中加载：tileSize*tileSize*N次
// 在本版本中，计算C中的一个tile需要从A和B中加载：tileSize * N次
// 从dram中加载数据量为原来的 1 / tileSize
template <int M, int N, int K>
void MatMulV2(const std::array<std::array<float, N>, M>& A,
              const std::array<std::array<float, K>, N>& B,
              std::array<std::array<float, K>, M>& C) {
  // 测试发现tile_size=4时，性能没有tile_size=8好
  // 如果把tile_size设置为16，则可能因为寄存器数量不足，导致性能极差
  constexpr int tile_size = 8;
  for (int i = 0; i < M; i += tile_size) {
    for (int k = 0; k < K; k += tile_size) {
      float tile[tile_size][tile_size] = {0};
      for (int j = 0; j < N; ++j) {
        // 将A[i:i+tileSize][j]和B[j][k:k+tileSize]缓存到寄存器上
        float register_a[tile_size];
        float register_b[tile_size];
        for (int t = 0; t < tile_size; ++t) {
          register_a[t] = A[i + t][j];
          register_b[t] = B[j][k + t];
        }
        // 计算tile的一个部分结果(1/j)
        for (int r = 0; r < tile_size; ++r) {
          for (int c = 0; c < tile_size; ++c) {
            tile[r][c] += register_a[r] * register_b[c];
          }
        }
      }
      // 把tile中的数据拷贝回C中
      for (int r = 0; r < tile_size; r++) {
        for (int c = 0; c < tile_size; c++) {
          C[i + r][k + c] = tile[r][c];
        }
      }
    }
  }
}

// V4版本为CacheLine的版本，先把A和B中的tile行或tile列加载到L1Cache
template <int M, int N, int K>
void MatMulV3(const std::array<std::array<float, N>, M>& A,
              const std::array<std::array<float, K>, N>& B,
              std::array<std::array<float, K>, M>& C) {
  constexpr int tile_size = 16;
  for (int i = 0; i < M; i += tile_size) {
    // line_cache = A[i:i+tileSize]
    float line_cache_a[tile_size][N];
    for (int r = 0; r < tile_size; ++r) {
      for (int c = 0; c < N; ++c) {
        line_cache_a[r][c] = A[i + r][c];
      }
    }
    for (int k = 0; k < K; k += tile_size) {
      float line_cache_b[N][tile_size];
      for (int r = 0; r < N; ++r) {
        for (int c = 0; c < tile_size; ++c) {
          line_cache_b[r][c] = B[r][k + c];
        }
      }
      float tile[tile_size][tile_size] = {0};
      for (int r = 0; r < tile_size; ++r) {
        for (int c = 0; c < tile_size; ++c) {
          for (int j = 0; j < N; ++j) {
            tile[r][c] += line_cache_a[r][j] * line_cache_b[j][c];
          }
        }
      }
      for (int r = 0; r < tile_size; r++) {
        for (int c = 0; c < tile_size; c++) {
          C[i + r][k + c] = tile[r][c];
        }
      }
    }
  }
}

template <int M, int N>
std::array<std::array<float, N>, M> Randn(float mean = 0, float var = 1) {
  // 创建一个正态分布的随机数生成器
  std::random_device rd;   // 随机数种子
  std::mt19937 gen(rd());  // Mersenne Twister 19937 生成器
  std::normal_distribution<float> dist(mean,
                                       var);  // 均值为0，标准差为1的正态分布
  std::array<std::array<float, N>, M> array_2d;
  for (auto& array : array_2d) {
    for (auto& v : array) {
      v = dist(gen);
    }
  }
  return array_2d;
}

int main() {
  constexpr int M = 256;
  constexpr int N = 256;
  constexpr int K = 256;
  auto A = Randn<M, N>();
  auto B = Randn<N, K>();
  auto C = Randn<M, K>();

  constexpr int num_runs = 100;

  // test MatMulV0~MatMulV3 performance
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_runs; ++i) {
    MatMulV0<M, N, K>(A, B, C);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cout << "MatMulV0: " << diff.count() << " s" << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_runs; ++i) {
    MatMulV1<M, N, K>(A, B, C);
  }
  end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  std::cout << "MatMulV1: " << diff.count() << " s" << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_runs; ++i) {
    MatMulV2<M, N, K>(A, B, C);
  }
  end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  std::cout << "MatMulV2: " << diff.count() << " s" << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_runs; ++i) {
    MatMulV3<M, N, K>(A, B, C);
  }
  end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  std::cout << "MatMulV3: " << diff.count() << " s" << std::endl;

  return 0;
}
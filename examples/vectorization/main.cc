#include <immintrin.h>  // 包含AVX2指令集的头文件
#include <omp.h>

#include <chrono>  // 包含计时器头文件
#include <iostream>
#include <vector>

void vector_add(float* a, float* b, float* c, int n) {
  for (int i{0}; i < n; ++i) {  // 处理剩余的元素
    c[i] = a[i] + b[i];
  }
}

void vector_add_avx2(float* a, float* b, float* c, int n) {
  int i;
  __m256 v_a, v_b, v_c;

#pragma omp parallel for private(v_a, v_b, v_c, i)  // 使用OpenMP并行化for循环
  for (i = 0; i < n; i += 8) {
    v_a = _mm256_loadu_ps(a + i);
    v_b = _mm256_loadu_ps(b + i);
    v_c = _mm256_add_ps(v_a, v_b);
    _mm256_storeu_ps(c + i, v_c);
  }

// 处理剩余的元素（如果存在）
#pragma omp parallel for private(i)  // 使用OpenMP并行化for循环
  for (i = (n / 8) * 8; i < n; i++) {
    c[i] = a[i] + b[i];
  }
}

int main() {
  const int n = 10000000;   // 指定向量长度
  const int num_runs = 10;  // 指定运行次数

  const std::size_t alignment = 32;
  alignas(alignment) std::vector<float> a(n);
  alignas(alignment) std::vector<float> b(n);
  alignas(alignment) std::vector<float> c(n);

  // 初始化a和b
  for (int i = 0; i < n; i++) {
    a[i] = i;
    b[i] = i;
  }

  // 测试vector_add函数性能
  double total_time = 0.0;  // 总运行时间
  for (int i = 0; i < num_runs; i++) {
    auto start_time =
        std::chrono::high_resolution_clock::now();     // 记录开始时间
    vector_add_avx2(a.data(), b.data(), c.data(), n);  // 进行向量加法运算
    auto end_time = std::chrono::high_resolution_clock::now();  // 记录结束时间

    // 计算运行时间
    std::chrono::duration<double, std::milli> elapsed_time =
        end_time - start_time;
    total_time += elapsed_time.count();
  }

  // 输出向量加法运算时间
  double avg_time = total_time / num_runs;  // 平均运行时间
  std::cout << "vector_add time: " << avg_time << "ms" << std::endl;

  return 0;
}

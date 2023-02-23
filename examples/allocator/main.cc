#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <random>
#include <stack>
#include <unordered_set>

// 提供一个malloc_allocator，内部直接使用malloc/free来进行内存分配和释放
// 避免全局的stack中的内存分配被统计到用户的内存使用
template <typename T>
struct malloc_allocator {
  typedef T value_type;
  typedef std::true_type is_always_equal;
  typedef std::true_type propagate_on_container_move_assignment;

  malloc_allocator() = default;
  template <typename U>
  malloc_allocator(const malloc_allocator<U>&) {}

  template <typename U>
  struct rebind {
    typedef malloc_allocator<U> other;
  };

  T* allocate(size_t n) { return static_cast<T*>(malloc(n * sizeof(T))); }
  void deallocate(T* p, size_t) { free(p); }
};

struct context {
  const char* file;
  const char* func;
};

// 线程级别的全局stack，用于保存显式保存的程序上下文
thread_local std::stack<context, std::deque<context, malloc_allocator<context>>>
    context_stack;

const context default_ctx{"<UNKOWN>", "<UNKOWN>"};

const context& GetCurrentContext() {
  if (context_stack.empty()) {
    return default_ctx;
  }
  return context_stack.top();
}

void RestoreContext() { context_stack.pop(); }

void SaveContext(const context& ctx) { context_stack.push(ctx); }

class CheckPoint {
 public:
  explicit CheckPoint(const context& ctx) { SaveContext(ctx); }
  ~CheckPoint() { RestoreContext(); }
};

// 生成一个调用位置的代码上下文，并保存到线程局面的stack中
#define MEMORY_CHECKPOINT()                   \
  CheckPoint func_checkpoint {                \
    context { __FILE__, __PRETTY_FUNCTION__ } \
  }

// 双向链表的表头
struct alloc_list_base {
  alloc_list_base* next;
  alloc_list_base* prev;
};

// 双向链表的结点，可以强制类型转换为alloc_list_base
// 结点中主要记录了当前内部分配的上下文和大小等信息
struct alloc_list_t : alloc_list_base {
  std::size_t size;
  context ctx;
  uint32_t head_size;
  uint32_t magic;
};

// 全局的双向链表，prev和next都指向自己
alloc_list_base alloc_list = {&alloc_list, &alloc_list};
// 全局内存使用统计
std::size_t current_mem_alloc = 0;

constexpr uint32_t CMT_MAGIC = 0x4D'58'54'43;  // "CTXM"

constexpr uint32_t align(std::size_t aligment, std::size_t s) {
  return static_cast<uint32_t>((s + aligment - 1) / aligment * aligment);
}

void* alloc_mem(std::size_t size, const context& ctx,
                std::size_t alignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
  // 将上下文记录块的大小也对齐到alignment，是为了返回给用户的usr_ptr也是对齐的
  uint32_t aligned_list_node_size = align(alignment, sizeof(alloc_list_t));
  // s为记录块加上用户需求size的大小
  std::size_t s = size + aligned_list_node_size;
  auto* ptr = static_cast<alloc_list_t*>(
      std::aligned_alloc(alignment, align(alignment, s)));
  if (ptr == nullptr) {
    return nullptr;
  }
  auto* usr_ptr = reinterpret_cast<std::byte*>(ptr) + aligned_list_node_size;
  ptr->ctx = ctx;
  ptr->size = size;
  ptr->head_size = aligned_list_node_size;
  ptr->magic = CMT_MAGIC;

  // 在alloc_list和alloc_list.prev之间插入新的结点
  ptr->prev = alloc_list.prev;
  ptr->next = &alloc_list;
  alloc_list.prev->next = ptr;
  alloc_list.prev = ptr;

  current_mem_alloc += size;

  return usr_ptr;
}

alloc_list_t* convert_user_ptr(void* usr_ptr, size_t alignment) {
  auto offset =
      static_cast<std::byte*>(usr_ptr) - static_cast<std::byte*>(nullptr);
  auto byte_ptr = static_cast<std::byte*>(usr_ptr);

  if (offset % alignment != 0) {
    return nullptr;
  }

  // usr_ptr向前减去一个对齐后的控制块的大小
  auto ptr = reinterpret_cast<alloc_list_t*>(
      byte_ptr - align(alignment, sizeof(alloc_list_t)));
  if (ptr->magic != CMT_MAGIC) {
    return nullptr;
  }

  return ptr;
}

void free_mem(void* usr_ptr,
              std::size_t alignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
  if (usr_ptr == nullptr) {
    return;
  }

  auto ptr = convert_user_ptr(usr_ptr, alignment);
  if (ptr == nullptr) {
    puts(
        "Invalid pointer or "
        "double-free");
    abort();
  }
  current_mem_alloc -= ptr->size;
  ptr->magic = 0;
  // 删除双向链表中的一个结点
  ptr->prev->next = ptr->next;
  ptr->next->prev = ptr->prev;
  free(ptr);
}

int check_leaks() {
  int leak_cnt = 0;
  auto ptr = static_cast<alloc_list_t*>(alloc_list.next);
  while (ptr != &alloc_list) {
    if (ptr->magic != CMT_MAGIC) {
      printf("error: heap data corrupt near %p\n", &ptr->magic);
      std::abort();
    }
    auto usr_ptr = reinterpret_cast<const std::byte*>(ptr) + ptr->head_size;
    printf("Leaked object at %p (size %zu, ", usr_ptr, ptr->size);
    printf("%s:%s", ptr->ctx.file, ptr->ctx.func);
    printf(")\n");
    ptr = static_cast<alloc_list_t*>(ptr->next);
  }
  if (leak_cnt) {
    printf("*** %d leaks found\n", leak_cnt);
  }

  return leak_cnt;
}

class invoke_check_leak_t {
 public:
  ~invoke_check_leak_t() { check_leaks(); }
} invoke_check_leak;

void* operator new(std::size_t size, const context& ctx) {
  void* ptr = alloc_mem(size, ctx);
  if (ptr) {
    return ptr;
  } else {
    throw std::bad_alloc();
  }
}

void* operator new[](std::size_t size, const context& ctx) {
  void* ptr = alloc_mem(size, ctx);
  if (ptr) {
    return ptr;
  } else {
    throw std::bad_alloc();
  }
}

void* operator new(std::size_t size, std::align_val_t align_val,
                   const context& ctx) {
  void* ptr = alloc_mem(size, ctx, static_cast<std::size_t>(align_val));
  if (ptr) {
    return ptr;
  } else {
    throw std::bad_alloc();
  }
}

void* operator new[](std::size_t size, std::align_val_t align_val,
                     const context& ctx) {
  void* ptr = alloc_mem(size, ctx, static_cast<std::size_t>(align_val));
  if (ptr) {
    return ptr;
  } else {
    throw std::bad_alloc();
  }
}

void operator delete(void* ptr, const context&) { free_mem(ptr); }

void operator delete[](void* ptr, const context&) { free_mem(ptr); }

void operator delete(void* ptr, std::align_val_t align_val, const context&) {
  free_mem(ptr, static_cast<std::size_t>(align_val));
}

void operator delete[](void* ptr, std::align_val_t align_val, const context&) {
  free_mem(ptr, static_cast<std::size_t>(align_val));
}

void* operator new(std::size_t size) {
  return operator new(size, GetCurrentContext());
}

void operator delete(void* ptr) { free_mem(ptr); }

void operator delete(void* ptr, std::size_t) { free_mem(ptr); }

void *ptr1, *ptr2;  // 防止被编译器优化

int main() {
  ptr1 = new char[10];
  MEMORY_CHECKPOINT();
  ptr2 = new char[20];
  return 0;
}
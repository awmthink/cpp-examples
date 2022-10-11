#include <gtest/gtest.h>

#include <iostream>
#include <thread>

void CounterFunction(int id, int num_iterations) {
  for (int i{}; i < num_iterations; ++i) {
    std::cout << "Counter " << id << " has value " << i << std::endl;
  }
}
TEST(ThreadTest, CreateThreadWithFunctionPointer) {
  std::thread t1{CounterFunction, 1, 6};
  std::thread t2{CounterFunction, 2, 4};
  t1.join();
  t2.join();
}

class CounterFunctor {
 public:
  CounterFunctor(int id, int num_iterations) : id_(id), num_iterations_(num_iterations) {}
  void operator()() const {
    for (int i{}; i < num_iterations_; ++i) {
      std::cout << "Counter " << id_ << " has value " << i << std::endl;
    }
  }

 private:
  int id_;
  int num_iterations_;
};

TEST(ThreadTest, CreateThreadWithFunctionObject) {
  std::thread t1{CounterFunctor{1, 6}};
  CounterFunctor c{2, 4};
  std::thread t2{c};

  t1.join();
  t2.join();
}

TEST(ThreadTest, CreateThreadWithLambda) {
  int id{1};
  int num_iterations{5};
  auto counter = [id, num_iterations]() {
    for (int i{}; i < num_iterations; ++i) {
      std::cout << "Counter " << id << " has value " << i << std::endl;
    }
  };
  std::thread t{counter};
  t.join();
}

class Widget {
 public:
  Widget(int id, int num_iterations) : id_(id), num_iterations_(num_iterations) {}
  void Counter() const {
    for (int i{}; i < num_iterations_; ++i) {
      std::cout << "Counter " << id_ << " has value " << i << std::endl;
    }
  }

 private:
  int id_;
  int num_iterations_;
};

TEST(ThreadTest, CreateThreadWithMemberFunction) {
  Widget w{1, 5};
  std::thread t{&Widget::Counter, &w};
  t.join();
}

// 本用例说明了std::thread对象，如果没有被任何显式的join或detach
// 则对象析构时，将会调用std::terminate
TEST(ThreadTest, RunThreadWithoutJoinAndDetach) {
  EXPECT_DEATH({ std::thread t{[]() { std::cout << "thread" << std::endl; }}; }, "");
}

TEST(ThreadTest, DoubleJoinOrDetach) {
  std::thread t{[]() { std::cout << "thread" << std::endl; }};
  t.join();
  // 重复join/detach会触发异常
  EXPECT_ANY_THROW(t.join());
  EXPECT_ANY_THROW(t.detach());
}

// 如果一个detached的线程发生了崩溃，则主进程也会崩溃
TEST(ThreadTest, DetachACorruptedThread) {
  EXPECT_DEATH(
      {
        std::thread{[]() { throw ""; }}.detach();
        std::this_thread::sleep_for(std::chrono::seconds(1));
      },
      "");
}

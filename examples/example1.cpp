// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

/*
 * Example use case of InlineBench using the 'INLINE_BENCHMARK_THREAD_WALL_*' macro:
 *  measure cpu time
 */

#include <inlinebench/inlinebench.h>

#include <thread>

int _fibonacci(int n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  }
  return _fibonacci(n - 1) + _fibonacci(n - 2);
}

int fibonacci(int n) {
  INLINE_BENCHMARK_WALL_START("computing fibonacci");
  int res = _fibonacci(n);
  INLINE_BENCHMARK_WALL_STOP("computing fibonacci");
  std::cout << "fibonacci(" << n << ") = " << res << std::endl;
  return res;
}

int main() {
  std::thread t0(fibonacci, 10);
  std::thread t1(fibonacci, 30);
  std::thread t2(fibonacci, 40);

  t0.join();
  t1.join();
  t2.join();

  std::cout << INLINE_BENCHMARK_REPORT("plain") << std::endl;
  return 0;
}
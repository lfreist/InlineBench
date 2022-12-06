// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

/*
 * Example use case of InlineBench using the 'INLINE_BENCHMARK_WALL_*' macro:
 *  measure wall time
 */

#include <inlinebench/inlinebench.h>

int fibonacci(int n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  }
  return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
  INLINE_BENCHMARK_WALL_START("computing fibonacci");
  int result = fibonacci(40);
  INLINE_BENCHMARK_WALL_STOP("computing fibonacci");
  std::cout << "fibonacci(40) = " << result << std::endl;
  std::cout << INLINE_BENCHMARK_REPORT("plain") << std::endl;
  return 0;
}
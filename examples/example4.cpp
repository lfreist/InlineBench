// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

/*
 * Example use case of InlineBench using different InlineBench macros
 */

#include <inlinebench/inlinebench.h>
#include <unistd.h>

#include <thread>

void wait_for(int seconds) {
  for (; seconds > 0; --seconds) {
    INLINE_BENCHMARK_CPU_START("sleep");
    INLINE_BENCHMARK_WALL_START("sleep");
    sleep(1);
    INLINE_BENCHMARK_WALL_STOP("sleep");
    INLINE_BENCHMARK_CPU_STOP("sleep");
  }
}

int main() {
  INLINE_BENCHMARK_WALL_START("Total Time running this program");
  std::cout << "sleeping for some time on multiple threads!" << std::endl;
  INLINE_BENCHMARK_CPU_START("total");
  std::thread t0(wait_for, 3);
  std::thread t1(wait_for, 5);
  std::thread t2(wait_for, 7);

  t0.join();
  t1.join();
  t2.join();
  INLINE_BENCHMARK_CPU_STOP("total");

  INLINE_BENCHMARK_WALL_STOP("Total Time running this program");
  std::cout << INLINE_BENCHMARK_REPORT("plain") << std::endl;
  return 0;
}
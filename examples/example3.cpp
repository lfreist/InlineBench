// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

/*
 * Example use case of InlineBench using the and 'INLINE_BENCHMARK_THREAD_CPU_*' macro:
 *  measure cpu time of the different threads
 */

#include <inlinebench/inlinebench.h>
#include <unistd.h>

#include <thread>

void wait_for(int seconds) {
  for (; seconds > 0; --seconds) {
    INLINE_BENCHMARK_THREAD_CPU_START("sleep");
    sleep(1);
    INLINE_BENCHMARK_THREAD_CPU_STOP("sleep");
  }
}

int main() {
  std::cout << "sleeping for 5 seconds... on multiple threads!" << std::endl;
  std::thread t0(wait_for, 5);
  std::thread t1(wait_for, 5);
  std::thread t2(wait_for, 5);

  t0.join();
  t1.join();
  t2.join();

  std::cout << INLINE_BENCHMARK_REPORT("plain") << std::endl;
  return 0;
}
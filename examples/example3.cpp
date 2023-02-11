// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

/*
 * Example use case of InlineBench using the and 'INLINE_BENCHMARK_THREAD_CPU_*' macro:
 *  measure cpu time of the different threads
 */

#include <inlinebench/inlinebench.h>
#include <unistd.h>

#include <thread>

void busy_wait_for(int seconds) {
  auto start = std::chrono::steady_clock::now();
  INLINE_BENCHMARK_CPU_START(_, sleep);
  while (true) {
    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() >= seconds) {
      break;
    }
  }
}

int main() {
  std::cout << "sleeping for 5 seconds... on multiple threads!" << std::endl;
  std::thread t0(busy_wait_for, 5);
  std::thread t1(busy_wait_for, 5);
  std::thread t2(busy_wait_for, 5);

  t0.join();
  t1.join();
  t2.join();

  std::cout << INLINE_BENCHMARK_REPORT(plain) << std::endl;
  return 0;
}
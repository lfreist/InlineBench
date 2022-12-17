// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

/*
 * Example use case of InlineBench using the and 'INLINE_BENCHMARK_TOTAL_CPU_*' macro:
 *  - measure cpu time
 */

#include <inlinebench/inlinebench.h>
#include <unistd.h>

int main() {
  std::cout << "Sleeping 5 seconds..." << std::endl;
  INLINE_BENCHMARK_CPU_START("sleep");
  sleep(5);
  INLINE_BENCHMARK_CPU_STOP("sleep");
  std::cout << INLINE_BENCHMARK_REPORT("plain") << std::endl;
  return 0;
}

// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

/*
 * Example use case of InlineBench using the scoped time measurement.
 */

#include <inlinebench/inlinebench.h>

#include <string>
#include <vector>
#include <random>

std::string generate_string(size_t size) {
  INLINE_BENCHMARK_WALL_START(_, generating string);  // this will stop measuring at the end of the scope automatically
  std::string str;
  str.reserve(size);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 25);

  for (size_t i = 0; i < size; ++i) {
    char c = 'a' + dis(gen);
    str.push_back(c);
  }

  return str;
}

int main() {
  int num_strings = 200;
  std::vector<std::string> strings(num_strings);
  for (int i = 0; i < num_strings; ++i) {
    INLINE_BENCHMARK_WALL_START(_, generating all string);
    std::string str = generate_string(256);
    INLINE_BENCHMARK_WALL_STOP(generating all string);
    strings.push_back(std::move(str));
  }
  std::cout << INLINE_BENCHMARK_REPORT("plain") << std::endl;
  return 0;
}
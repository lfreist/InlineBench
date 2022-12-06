// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <inlinebench/benchmark.h>

#include <string>
#include <vector>

class InlineBenchmarkRegistrator {
 public:
  static void start(const std::string& name, uint8_t bm_t_id) {
    InlineBenchmarkHandler& instance = InlineBenchmarkHandler::GetInstance();
    instance.start(name, bm_t_id);
  }

  static void stop(const std::string& name, uint8_t bm_t_id) {
    InlineBenchmarkHandler& instance = InlineBenchmarkHandler::GetInstance();
    instance.stop(name, bm_t_id);
  }

  static std::string report(const std::string& fmt) {
    InlineBenchmarkHandler& instance = InlineBenchmarkHandler::GetInstance();
    return instance.Report(fmt);
  }
};

#ifdef BENCHMARK
#define INLINE_BENCHMARK_THREAD_CPU_START(name) InlineBenchmarkRegistrator::start(name, 0)
#define INLINE_BENCHMARK_THREAD_CPU_STOP(name) InlineBenchmarkRegistrator::stop(name, 0)

#define INLINE_BENCHMARK_TOTAL_CPU_START(name) InlineBenchmarkRegistrator::start(name, 1)
#define INLINE_BENCHMARK_TOTAL_CPU_STOP(name) InlineBenchmarkRegistrator::stop(name, 1)

#define INLINE_BENCHMARK_THREAD_WALL_START(name) InlineBenchmarkRegistrator::start(name, 2)
#define INLINE_BENCHMARK_THREAD_WALL_STOP(name) InlineBenchmarkRegistrator::stop(name, 2)

#define INLINE_BENCHMARK_WALL_START(name) InlineBenchmarkRegistrator::start(name, 3)
#define INLINE_BENCHMARK_WALL_STOP(name) InlineBenchmarkRegistrator::stop(name, 3)

#define INLINE_BENCHMARK_REPORT(fmt) InlineBenchmarkRegistrator::report(fmt)
#else
// empty definitions
#define INLINE_BENCHMARK_THREAD_CPU_START(name)
#define INLINE_BENCHMARK_THREAD_CPU_STOP(name)

#define INLINE_BENCHMARK_TOTAL_CPU_START(name)
#define INLINE_BENCHMARK_TOTAL_CPU_STOP(name)

#define INLINE_BENCHMARK_THREAD_WALL_START(name)
#define INLINE_BENCHMARK_THREAD_WALL_STOP(name)

#define INLINE_BENCHMARK_WALL_START(name)
#define INLINE_BENCHMARK_WALL_STOP(name)

#define INLINE_BENCHMARK_REPORT(fmt) ""
#endif
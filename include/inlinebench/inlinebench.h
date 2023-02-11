// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <pthread.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#if defined(unix) || defined(__unix) || defined(__unix__)
#define __IB_UNIX__
#include <ctime>
#elif defined(__APPLE__)
#define __IB_APPLE__
#elif defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define __IB_WINDOWS__
#endif

// ==== CPU Timer ======================================================================================================
class CPUTime {
 public:
#if defined(__IB_UNIX__)
  static int64_t now() {
    struct timespec ts {};
    pthread_t pth_t = pthread_self();
    clockid_t clock_id;
    pthread_getcpuclockid(pth_t, &clock_id);
    if (::clock_gettime(clock_id, &ts)) {
      throw std::runtime_error("CPUTime: Internal Error");
    }
    return (ts.tv_sec * 1000000000) + ts.tv_nsec;
  }
#elif defined(__IB_APPLE__)
  static int64_t now() { return -1; }
#elif defined(__IB_WINDOWS__)
  static int64_t now() { return -1; }
#endif
};

// ==== implementations ================================================================================================
/**
 * dynamic code benchmark base class
 * @tparam TimePoint
 */
template <typename TimePoint>
class InlineBenchmark {
  template <typename T>
  friend std::ostream& operator<<(std::ostream& os, const InlineBenchmark<T>& c_bm);
  friend class InlineBenchmarkHandler;

 public:
  InlineBenchmark() = default;

  InlineBenchmark(const InlineBenchmark&) = delete;
  InlineBenchmark(InlineBenchmark&&) = delete;
  InlineBenchmark& operator=(const InlineBenchmark&) = delete;
  InlineBenchmark& operator=(InlineBenchmark&&) = delete;

  virtual void start() = 0;
  virtual void stop() = 0;
  [[nodiscard]] virtual std::map<std::thread::id, int64_t> getResults() const = 0;

 protected:
  std::mutex _pushToResultPairsMutex;
  std::map<std::thread::id, std::vector<std::pair<TimePoint, TimePoint>>> _resultPairs;
};

/**
 * CPU Timer
 */
class InlineBenchmarkCPU : public InlineBenchmark<int64_t> {
 public:
  InlineBenchmarkCPU() = default;

  InlineBenchmarkCPU(const InlineBenchmarkCPU&) = delete;
  InlineBenchmarkCPU(InlineBenchmarkCPU&&) = delete;
  InlineBenchmarkCPU& operator=(const InlineBenchmarkCPU&) = delete;
  InlineBenchmarkCPU& operator=(InlineBenchmarkCPU&&) = delete;

  void start() override {
    std::unique_lock locker(_pushToResultPairsMutex);
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    if (res_vector.empty()) {
      res_vector.emplace_back(CPUTime::now(), 0);
      return;
    }
    if (res_vector.back().second == 0) {
      throw std::runtime_error(
          "Starting dynamic benchmark failed, since there already is a timer "
          "running for this thread on this benchmark id");
    } else {
      res_vector.emplace_back(CPUTime::now(), 0);
    }
  };

  void stop() override {
    std::unique_lock locker(_pushToResultPairsMutex);
    auto now = CPUTime::now();
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    if (res_vector.empty()) {
      return;
    }
    auto& pair = res_vector.back();
    if (pair.second == 0) {
      pair.second = now;
    }
  };

  [[nodiscard]] std::map<std::thread::id, int64_t> getResults() const override {
    std::map<std::thread::id, int64_t> result;
    for (const auto& [thread_id, intervals] : _resultPairs) {
      auto& value = result[thread_id];
      value = 0;
      for (const auto& [start, end] : intervals) {
        value += end - start;
      }
    }
    return result;
  };
};

class InlineBenchmarkWall : public InlineBenchmark<std::chrono::time_point<std::chrono::steady_clock>>

{
 public:
  InlineBenchmarkWall() = default;

  InlineBenchmarkWall(const InlineBenchmarkWall&) = delete;
  InlineBenchmarkWall(InlineBenchmarkWall&&) = delete;
  InlineBenchmarkWall& operator=(const InlineBenchmarkWall&) = delete;
  InlineBenchmarkWall& operator=(InlineBenchmarkWall&&) = delete;

  void start() override {
    std::unique_lock locker(_pushToResultPairsMutex);
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    if (res_vector.empty()) {
      res_vector.emplace_back(std::chrono::steady_clock::now(),
                              std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>{});
      return;
    }
    if (res_vector.back().second == std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>{}) {
      throw std::runtime_error(
          "Starting dynamic benchmark failed, since there already is a timer "
          "running for this thread on this benchmark id");
    } else {
      res_vector.emplace_back(std::chrono::steady_clock::now(),
                              std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>{});
    }
  };

  void stop() override {
    std::unique_lock locker(_pushToResultPairsMutex);
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    if (res_vector.empty()) {
      return;
    }
    auto& pair = res_vector.back();
    if (pair.second == std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>{}) {
      pair.second = std::chrono::steady_clock::now();
    }
  };

  [[nodiscard]] std::map<std::thread::id, int64_t> getResults() const override {
    std::map<std::thread::id, int64_t> result;
    for (const auto& [thread_id, intervals] : _resultPairs) {
      // MARK: look up is O(log n) -> this reference might be super unnecessary...
      auto& value = result[thread_id];
      value = 0;
      for (const auto& [start, end] : intervals) {
        value += (end - start).count();
      }
    }
    return result;
  }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const InlineBenchmark<T>& c_bm) {
  os << c_bm._resultPairs.size() << " " << (c_bm._resultPairs.size() == 1 ? "thread" : "threads") << ":\n";
  double total = 0;
  for (const auto& [thread_id, time] : c_bm.getResults()) {
    total += time;
    if (c_bm._resultPairs.size() > 1) {
      os << "  " << time / 1000.0 / 1000.0 << " ms\n";
    }
  }
  if (c_bm._resultPairs.size() > 1) {
    os << "  --------\n";
  }
  os << "  " << total / 1000.0 / 1000.0 << " ms" << std::endl;
  return os;
}

// ==== handler ========================================================================================================
enum BenchmarkType { CPU, WALL };

class InlineBenchmarkHandler {
  friend class InlineBenchmarkRegistrator;

 public:
  static InlineBenchmarkHandler& GetInstance() {
    static InlineBenchmarkHandler instance;
    return instance;
  };

  [[nodiscard]] std::string Report(const std::string& fmt = "plain") const {
    if (fmt == "csv") {
      const std::string sep(",");
      std::stringstream ss;
      unsigned max_size = 0;
      for (const auto& [name, bm] : _CPU_benchmarks) {
        auto tmp_size = bm.getResults().size();
        max_size = tmp_size > max_size ? tmp_size : max_size;
      }
      for (const auto& [name, bm] : _Wall_benchmarks) {
        auto tmp_size = bm.getResults().size();
        max_size = tmp_size > max_size ? tmp_size : max_size;
      }
      ss << "bm_type" << sep << "name";
      for (unsigned i = 0; i < max_size; ++i) {
        ss << sep << i;
      }
      ss << '\n';
      for (const auto& [name, bm] : _CPU_benchmarks) {
        ss << "total CPU [ns]" << sep << name;
        unsigned nums = max_size;
        for (const auto& [thread_id, time] : bm.getResults()) {
          ss << sep << std::llround(time);
          nums--;
        }
        for (; nums > 0; --nums) {
          ss << sep;
        }
        ss << '\n';
      }
      for (const auto& [name, bm] : _Wall_benchmarks) {
        ss << "thread Wall [ns]" << sep << name;
        unsigned nums = max_size;
        for (const auto& [thread_id, time] : bm.getResults()) {
          ss << sep << std::llround(time);
          nums--;
        }
        for (; nums > 0; --nums) {
          ss << sep;
        }
        ss << '\n';
      }
      return ss.str();
    } else if (fmt == "json") {
      std::string ret("{\n\"CPU\": {");
      for (const auto& [name, bm] : _CPU_benchmarks) {
        ret.append("\n  \"" + name + "\": {");
        int id_counter = 0;
        for (const auto& [thread_id, time] : bm.getResults()) {
          std::stringstream id;
          id << thread_id;
          ret.append("\n    \"" + std::to_string(id_counter) + "\":{\n      \"id\": " + id.str() +
                     ",\n      \"time\": " + std::to_string(time) + "\n    },");
          id_counter++;
        }
        ret.pop_back();
        ret.append("\n  },");
      }
      ret.append("\n},\n\"Wall\": {");
      for (const auto& [name, bm] : _Wall_benchmarks) {
        ret.append("\n  \"" + name + "\": {");
        int id_counter = 0;
        for (const auto& [thread_id, time] : bm.getResults()) {
          std::stringstream id;
          id << thread_id;
          ret.append("\n    \"" + std::to_string(id_counter) + "\":{\n      \"id\": " + id.str() +
                     ",\n      \"time\": " + std::to_string(time) + "\n    },");
          id_counter++;
        }
        ret.pop_back();
        ret.append("\n  },");
      }
      ret.pop_back();
      ret.append("\n}\n}");
      return ret;
    } else {
      std::stringstream ss;
      ss << "CPU:\n";
      for (const auto& [name, bm] : _CPU_benchmarks) {
        ss << name << " - " << bm << std::endl;
      }
      ss << "Wall:\n";
      for (const auto& [name, bm] : _Wall_benchmarks) {
        ss << name << " - " << bm << std::endl;
      }
      return ss.str();
    }
  };

  void start(const std::string& name, BenchmarkType bm_t_id) {
    switch (bm_t_id) {
      case CPU:
        _CPU_benchmarks[name].start();
        break;
      case WALL:
        _Wall_benchmarks[name].start();
        break;
      default:
        break;
    }
  };

  void stop(const std::string& name, BenchmarkType bm_t_id) {
    switch (bm_t_id) {
      case CPU:
        _CPU_benchmarks[name].stop();
        break;
      case WALL:
        _Wall_benchmarks[name].stop();
        break;
      default:
        break;
    }
  };

 private:
  InlineBenchmarkHandler() = default;

  std::map<const std::string, InlineBenchmarkCPU> _CPU_benchmarks;
  std::map<const std::string, InlineBenchmarkWall> _Wall_benchmarks;
};

// ==== registrator ====================================================================================================
class InlineBenchmarkRegistrator {
 public:
  InlineBenchmarkRegistrator(std::string name, BenchmarkType bm_t_id) : _name(std::move(name)), _bm_t_id(bm_t_id) {
    InlineBenchmarkRegistrator::start(_name, _bm_t_id);
  }

  ~InlineBenchmarkRegistrator() {
    InlineBenchmarkRegistrator::stop(_name, _bm_t_id);
  }

  static void start(const std::string& name, BenchmarkType bm_t_id) {
    InlineBenchmarkHandler& instance = InlineBenchmarkHandler::GetInstance();
    instance.start(name, bm_t_id);
  }

  static void stop(const std::string& name, BenchmarkType bm_t_id) {
    InlineBenchmarkHandler& instance = InlineBenchmarkHandler::GetInstance();
    instance.stop(name, bm_t_id);
  }

  static std::string report(const std::string& fmt) {
    InlineBenchmarkHandler& instance = InlineBenchmarkHandler::GetInstance();
    return instance.Report(fmt);
  }

 private:
  std::string _name;
  BenchmarkType _bm_t_id;
};

// ==== MACROS =========================================================================================================

#ifdef BENCHMARK

#define concatArgs(...) \
    std::string(__VA_ARGS__).empty() ? "" : (" " + std::string(__VA_ARGS__)) + concatArgs1(__VA_ARGS__)
#define concatArgs1(...) \
    std::string(__VA_ARGS__).empty() ? "" : (" " + std::string(__VA_ARGS__)) + concatArgs(__VA_ARGS__)

#define INLINE_BENCHMARK_CPU_START(var, name, ...) \
  InlineBenchmarkRegistrator var = InlineBenchmarkRegistrator(std::string(#name), CPU)
#define INLINE_BENCHMARK_CPU_STOP(name) \
  InlineBenchmarkRegistrator::stop(std::string(#name), CPU)
#define INLINE_BENCHMARK_WALL_START(var, name) \
  InlineBenchmarkRegistrator var = InlineBenchmarkRegistrator(std::string(#name), WALL)
#define INLINE_BENCHMARK_WALL_STOP(name) \
  InlineBenchmarkRegistrator::stop(std::string(#name), WALL)

#define INLINE_BENCHMARK_REPORT(fmt) InlineBenchmarkRegistrator::report(#fmt)
#else
// empty definitions
#define INLINE_BENCHMARK_CPU_START(name)
#define INLINE_BENCHMARK_CPU_STOP(name)

#define INLINE_BENCHMARK_WALL_START(name)
#define INLINE_BENCHMARK_WALL_STOP(name)

#define INLINE_BENCHMARK_REPORT(fmt) ""
#endif
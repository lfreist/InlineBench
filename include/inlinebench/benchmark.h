// Copyright 2022, Leon Freist
// Author: Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <boost/chrono.hpp>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

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
  virtual std::map<std::thread::id, double> getResults() const = 0;

 protected:
  std::mutex _pushToResultPairsMutex;
  std::map<std::thread::id, std::vector<std::pair<TimePoint, TimePoint>>> _resultPairs;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const InlineBenchmark<T>& c_bm) {
  os << c_bm._resultPairs.size() << " " << (c_bm._resultPairs.size() == 1 ? "thread" : "threads") << ":\n";
  double total = 0;
  for (const auto& res : c_bm.getResults()) {
    total += res.second;
    if (c_bm._resultPairs.size() > 1) {
      os << "  " << res.second / 1000.0 / 1000.0 << " ms\n";
    }
  }
  if (c_bm._resultPairs.size() > 1) {
    os << "  --------\n";
  }
  os << "  " << total / 1000.0 / 1000.0 << " ms" << std::endl;
  return os;
}

class InlineBenchmarkThreadCPU : public InlineBenchmark<boost::chrono::thread_clock::time_point> {
 public:
  InlineBenchmarkThreadCPU() = default;

  InlineBenchmarkThreadCPU(const InlineBenchmarkThreadCPU&) = delete;
  InlineBenchmarkThreadCPU(InlineBenchmarkThreadCPU&&) = delete;
  InlineBenchmarkThreadCPU& operator=(const InlineBenchmarkThreadCPU&) = delete;
  InlineBenchmarkThreadCPU& operator=(InlineBenchmarkThreadCPU&&) = delete;

  void start() override {
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    auto now = boost::chrono::thread_clock::now();
    if (res_vector.empty()) {
      res_vector.push_back({now, now});
      return;
    }
    if (res_vector.back().second == res_vector.back().first) {
      throw std::runtime_error(
          "Starting dynamic benchmark failed, since there already is a timer "
          "running for this thread on this benchmark id");
    } else {
      res_vector.push_back({now, now});
    }
  };

  void stop() override {
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto now = boost::chrono::thread_clock::now();
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    if (res_vector.empty()) {
      return;
    }
    auto& pair = res_vector.back();
    if (pair.second == pair.first) {
      pair.second = now;
    } else {
      throw std::runtime_error(
          "Stopping code benchmark failed, since this benchmark id has not "
          "started a timer on this thread yet.");
    }
  };

  std::map<std::thread::id, double> getResults() const override {
    std::map<std::thread::id, double> result;
    for (const auto& res : _resultPairs) {
      double value = 0;
      for (const auto& interval : res.second) {
        value += double((interval.second - interval.first).count());
      }
      result[res.first] = value;
    }
    return result;
  };
};

class InlineBenchmarkTotalCPU : public InlineBenchmark<std::clock_t> {
 public:
  InlineBenchmarkTotalCPU() = default;

  InlineBenchmarkTotalCPU(const InlineBenchmarkTotalCPU&) = delete;
  InlineBenchmarkTotalCPU(InlineBenchmarkTotalCPU&&) = delete;
  InlineBenchmarkTotalCPU& operator=(const InlineBenchmarkTotalCPU&) = delete;
  InlineBenchmarkTotalCPU& operator=(InlineBenchmarkTotalCPU&&) = delete;

  void start() override {
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto& res_vector = _resultPairs[std::thread::id(0)];
    auto now = std::clock();
    if (res_vector.empty()) {
      res_vector.push_back({now, now});
      return;
    }
    if (res_vector.back().second == res_vector.back().first) {
      throw std::runtime_error(
          "Starting dynamic benchmark failed, since there already is a timer "
          "running for this thread on this benchmark id");
    } else {
      res_vector.push_back({now, now});
    }
  };

  void stop() override {
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto now = std::clock();
    auto& res_vector = _resultPairs[std::thread::id(0)];
    if (res_vector.empty()) {
      return;
    }
    auto& pair = res_vector.back();
    if (pair.second == pair.first) {
      pair.second = now;
    } else {
      throw std::runtime_error(
          "Stopping code benchmark failed, since this benchmark id has not "
          "started a timer on this thread yet.");
    }
  };

  std::map<std::thread::id, double> getResults() const override {
    std::map<std::thread::id, double> result;
    // only one single element with thread::id == 0 is available here...
    for (const auto& res : _resultPairs) {
      double value = 0;
      for (const auto& interval : res.second) {
        value += double((interval.second - interval.first)) / CLOCKS_PER_SEC;
      }
      value *= 1000 * 1000 * 1000;
      result[res.first] = value;
    }
    return result;
  };
};

class InlineBenchmarkThreadWall : public InlineBenchmark<std::chrono::time_point<std::chrono::steady_clock>>

{
 public:
  InlineBenchmarkThreadWall() = default;

  InlineBenchmarkThreadWall(const InlineBenchmarkThreadWall&) = delete;
  InlineBenchmarkThreadWall(InlineBenchmarkThreadWall&&) = delete;
  InlineBenchmarkThreadWall& operator=(const InlineBenchmarkThreadWall&) = delete;
  InlineBenchmarkThreadWall& operator=(InlineBenchmarkThreadWall&&) = delete;

  void start() override {
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    auto now = std::chrono::steady_clock::now();
    if (res_vector.empty()) {
      res_vector.push_back({now, now});
      return;
    }
    if (res_vector.back().second == res_vector.back().first) {
      throw std::runtime_error(
          "Starting dynamic benchmark failed, since there already is a timer "
          "running for this thread on this benchmark id");
    } else {
      res_vector.push_back({now, now});
    }
  };

  void stop() override {
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto now = std::chrono::steady_clock::now();
    auto& res_vector = _resultPairs[std::this_thread::get_id()];
    if (res_vector.empty()) {
      return;
    }
    auto& pair = res_vector.back();
    if (pair.second == pair.first) {
      pair.second = now;
    } else {
      throw std::runtime_error(
          "Stopping code benchmark failed, since this benchmark id has not "
          "started a timer on this thread yet.");
    }
  };

  std::map<std::thread::id, double> getResults() const override {
    std::map<std::thread::id, double> result;
    for (const auto& res : _resultPairs) {
      double value = 0;
      for (const auto& interval : res.second) {
        value += double((interval.second - interval.first).count());
      }
      result[res.first] = value;
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
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto& res_vector = _resultPairs[std::thread::id(0)];
    auto now = std::chrono::steady_clock::now();
    if (res_vector.empty()) {
      res_vector.push_back({now, now});
      return;
    }
    if (res_vector.back().second == res_vector.back().first) {
      throw std::runtime_error(
          "Starting dynamic benchmark failed, since there already is a timer "
          "running for this benchmark id");
    } else {
      res_vector.push_back({now, now});
    }
  };

  void stop() override {
    std::unique_lock<std::mutex> locker(_pushToResultPairsMutex);
    auto now = std::chrono::steady_clock::now();
    auto& res_vector = _resultPairs[std::thread::id(0)];
    if (res_vector.empty()) {
      return;
    }
    auto& pair = res_vector.back();
    if (pair.second == pair.first) {
      pair.second = now;
    } else {
      throw std::runtime_error(
          "Stopping code benchmark failed, since this benchmark id has not "
          "started a timer on this thread yet.");
    }
  };

  std::map<std::thread::id, double> getResults() const override {
    std::map<std::thread::id, double> result;
    // only one single element with thread::id == 0 is in this map...
    for (const auto& res : _resultPairs) {
      double value = 0;
      for (const auto& interval : res.second) {
        value += double((interval.second - interval.first).count());
      }
      result[res.first] = value;
    }
    return result;
  };
};

class InlineBenchmarkHandler {
  friend class InlineBenchmarkRegistrator;

 public:
  static InlineBenchmarkHandler& GetInstance() {
    static InlineBenchmarkHandler instance;
    return instance;
  };

  std::string Report(const std::string& fmt = "plain") const {
    const std::string sep(",");
    if (fmt == "csv") {
      std::stringstream ss;
      unsigned max_size = 0;
      for (const auto& bm : _threadCPU_benchmarks) {
        auto tmp_size = bm.second.getResults().size();
        max_size = tmp_size > max_size ? tmp_size : max_size;
      }
      for (const auto& bm : _totalCPU_benchmarks) {
        auto tmp_size = bm.second.getResults().size();
        max_size = tmp_size > max_size ? tmp_size : max_size;
      }
      for (const auto& bm : _threadWall_benchmarks) {
        auto tmp_size = bm.second.getResults().size();
        max_size = tmp_size > max_size ? tmp_size : max_size;
      }
      for (const auto& bm : _wall_benchmarks) {
        auto tmp_size = bm.second.getResults().size();
        max_size = tmp_size > max_size ? tmp_size : max_size;
      }
      ss << "bm_type" << sep << "name";
      for (unsigned i = 0; i < max_size; ++i) {
        ss << sep << i;
      }
      ss << '\n';
      for (const auto& bm : _threadCPU_benchmarks) {
        ss << "thread CPU [ns]" << sep << bm.first;
        unsigned nums = max_size;
        for (const auto& res : bm.second.getResults()) {
          ss << sep << std::llround(res.second);
          nums--;
        }
        for (; nums > 0; --nums) {
          ss << sep;
        }
        ss << '\n';
      }
      for (const auto& bm : _totalCPU_benchmarks) {
        ss << "total CPU [ns]" << sep << bm.first;
        unsigned nums = max_size;
        for (const auto& res : bm.second.getResults()) {
          ss << sep << std::llround(res.second);
          nums--;
        }
        for (; nums > 0; --nums) {
          ss << sep;
        }
        ss << '\n';
      }
      for (const auto& bm : _threadWall_benchmarks) {
        ss << "thread Wall [ns]" << sep << bm.first;
        unsigned nums = max_size;
        for (const auto& res : bm.second.getResults()) {
          ss << sep << std::llround(res.second);
          nums--;
        }
        for (; nums > 0; --nums) {
          ss << sep;
        }
        ss << '\n';
      }
      for (const auto& bm : _wall_benchmarks) {
        ss << "Wall [ns]" << sep << bm.first;
        unsigned nums = max_size;
        for (const auto& res : bm.second.getResults()) {
          ss << sep << std::llround(res.second);
          nums--;
        }
        for (; nums > 0; --nums) {
          ss << sep;
        }
        ss << '\n';
      }
      return ss.str();
    } else {
      std::stringstream ss;
      if (!_threadCPU_benchmarks.empty()) {
        ss << "Thread CPU:\n";
        for (const auto& bm : _threadCPU_benchmarks) {
          ss << bm.first << " - " << bm.second << std::endl;
        }
      }
      if (!_totalCPU_benchmarks.empty()) {
        ss << "Total CPU:\n";
        for (const auto& bm : _totalCPU_benchmarks) {
          ss << bm.first << " - " << bm.second << std::endl;
        }
      }
      if (!_threadWall_benchmarks.empty()) {
        ss << "Thread Wall:\n";
        for (const auto& bm : _threadWall_benchmarks) {
          ss << bm.first << " - " << bm.second << std::endl;
        }
      }
      if (!_wall_benchmarks.empty()) {
        ss << "Wall:\n";
        for (const auto& bm : _wall_benchmarks) {
          ss << bm.first << " - " << bm.second << std::endl;
        }
      }
      return ss.str();
    }
  };

  void start(const std::string& name, uint8_t _bm_t_id) {
    switch (_bm_t_id) {
      case 0:
        _threadCPU_benchmarks[name].start();
        break;
      case 1:
        _totalCPU_benchmarks[name].start();
        break;
      case 2:
        _threadWall_benchmarks[name].start();
        break;
      case 3:
        _wall_benchmarks[name].start();
        break;
      default:
        break;
    }
  };

  void stop(const std::string& name, uint8_t _bm_t_id) {
    switch (_bm_t_id) {
      case 0:
        _threadCPU_benchmarks[name].stop();
        break;
      case 1:
        _totalCPU_benchmarks[name].stop();
        break;
      case 2:
        _threadWall_benchmarks[name].stop();
        break;
      case 3:
        _wall_benchmarks[name].stop();
        break;
      default:
        break;
    }
  };

 private:
  InlineBenchmarkHandler() = default;

  std::map<const std::string, InlineBenchmarkThreadCPU> _threadCPU_benchmarks;
  std::map<const std::string, InlineBenchmarkTotalCPU> _totalCPU_benchmarks;
  std::map<const std::string, InlineBenchmarkThreadWall> _threadWall_benchmarks;
  std::map<const std::string, InlineBenchmarkWall> _wall_benchmarks;
};

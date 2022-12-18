[![clang](https://github.com/lfreist/InlineBench/actions/workflows/clang.yml/badge.svg)](https://github.com/lfreist/InlineBench/actions/workflows/clang.yml)
[![gcc](https://github.com/lfreist/InlineBench/actions/workflows/gcc.yml/badge.svg)](https://github.com/lfreist/InlineBench/actions/workflows/gcc.yml)

[![gcc](https://github.com/lfreist/InlineBench/actions/workflows/clang-format.yml/badge.svg)](https://github.com/lfreist/InlineBench/actions/workflows/clang-format.yml)


# InlineBench
InlineBench is a C++ header-only Library/Framework (c++11 standard) for dynamically benchmarking code fragments.

> Any questions, concerns, feedback, ideas or issues? Do not hesitate to [create an issue](https://github.com/lfreist/InlineBench/issues/new).

## Why InlineBench?

> *SUMMARY*: You can start and stop runtime measurements *everywhere* within your code. When compiling without `-DBENCHMARK` flag, the result is just like you never used *InlineBench*. Thus, you can benchmark critical parts of your code in *production-like* environment and find the bottleneck within your program easier.

There are a lot of very large and powerful benchmarking frameworks out there (e.g. [GoogleTest](https://github.com/google/googletest), [CppBenchmark](https://github.com/google/googletest), ...).
So why and for what should you use *InlineBench*?

*InlineBench* provides an interface to benchmark fragments of you code and programs in production scenarios.
Instead of micro-benchmarking functions or methods, you can just place the *InlineBench* specific macros within you code, and compile it with the `-DBENCHMARK` flag to activate benchmarks.
If you compile your code without this flag, all *InlineBench* macros will become empty statements and your compiler ignores them.
Thus, you can benchmark very specific parts of your code.
Not only functions but also only a couple of lines within functions etc.
*InlineBench* also supports thread specific measurements.
You can measure the CPU and Wall time of single threads or of your whole system.

Checkout the section [Features and Examples](#features-and-examples) for more information!

## Include *InlineBench* to Your Project
You can either just include the [`inlinebench.h`](https://github.com/lfreist/InlineBench/tree/main/include/inlinebench/inlinebench.h) header into your files.

> In this case **you must** link libbost-chrono when compiling!

Or you use the cmake interface library provided (`InlineBench`).

## Features and examples

### General
1. You need to import `inlinebench/inlinebench.h`
2. You need to compile your code with the `-DBENCHMARK` flag
3. If you start an *InlineBench* measurement, it expects you to stop it as well. Hence, for any `*_START()` macro, you need to place the corresponding `*_STOP` macro later in your code.

We will cover the details in the next subsections!

### Getting the Measurement Results
You can get the measurement results using `INLINE_BENCHMARK_REPORT(<type>)` with `<type>` being one of the following:
- "plain": plaint text formatting (easy to read)
- "csv": csv formatting
- "json": json formatting (not yet implemented!)

This macro is replaced by a function returning a `std::string`.
Thus, if you want the output in your results to the terminal, just do:
```c++
std::cout << INLINE_BENCHMARK_REPORT("plain");
```
If you rather want to write the output to a file, do:
```c++
std::ofstream file('path/to/file');
file << INLINE_BENCHMARK_REPORT("csv");
```

### Measuring Wall Time
Wall time is the 'real' time (like "How long did that just take?").
Thus, if *InlineBench* reports wall timings, the values are just the time you would measure with a stop watch.
The macros for measuring wall time are:
```c++
INLINE_BENCH_WALL_START("name of the measurement");
INLINE_BENCH_WALL_STOP("name of the measurement");
```
In between these macro calls, you place the code, that you want to benchmark.

#### Example
This example measures the wall time of the computation of `fibonacci(40)`:
```c++
INLINE_BENCH_WALL_START("name of the measurement");
int res = fibonacci(40);
INLINE_BENCH_WALL_STOP("name of the measurement");
```
A detailed and running example can be found [here](https://github.com/lfreist/InlineBench/tree/main/examples/example0.cpp).

### Measuring CPU Time
We image, that we run multiple threads on some code.
If we want to know, how long the different threads spent running the code, we simply use this macro:
```cpp
INLINE_BENCH_CPU_START("name of the measurement");
INLINE_BENCH_CPU_STOP("name of the measurement");
```

#### Example
```cpp
INLINE_BENCH_CPU_START("name of the measurement");
sleep(5);
INLINE_BENCH_CPU_STOP("name of the measurement");
```
A detailed and running example can be found [here](https://github.com/lfreist/InlineBench/tree/main/examples/example3.cpp).

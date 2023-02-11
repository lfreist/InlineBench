cmake_minimum_required(VERSION 3.9)
project(InlineBench C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_BENCHMARK "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DBENCHMARK")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include/)

add_library(InlineBench INTERFACE)
target_include_directories(InlineBench INTERFACE .)

IF(${MAIN_PROJECT})
    add_subdirectory(examples)
    include(CTest)

    add_test(Ex0 examples/Example0)
    add_test(Ex1 examples/Example1)
    add_test(Ex2 examples/Example2)
    add_test(Ex3 examples/Example3)
    add_test(ExScope examples/ExampleScope)
ENDIF()
cmake_minimum_required(VERSION 3.9)
project(InlineBench C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

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

# --- BOOST ------------------------------------------------------------------------------------------------------------
find_package(Boost 1.74 REQUIRED COMPONENTS chrono)
include_directories(${Boost_INCLUDE_DIR})
# ----------------------------------------------------------------------------------------------------------------------

include_directories(${PROJECT_SOURCE_DIR}/include/)

add_library(InlineBench INTERFACE)
target_include_directories(InlineBench INTERFACE .)
target_link_libraries(InlineBench INTERFACE boost_chrono)

IF(${MAIN_PROJECT})
    add_subdirectory(examples)
ENDIF()
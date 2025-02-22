cmake_minimum_required(VERSION 3.10)

find_package(fmt REQUIRED)
find_package(Folly REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(gflags REQUIRED)


set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${SRC_DIR})
file(GLOB SRC_FILES ${SRC_DIR}/*.cc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# C++ Benchmark
add_executable(bulldozer_bench bulldozer_bench.cc ${SRC_FILES})
target_link_libraries(bulldozer_bench
            PRIVATE
            Folly::folly Folly::follybenchmark
            fmt::fmt
            Boost::filesystem Boost::system
            gflags
            pthread
            wolfssl
)
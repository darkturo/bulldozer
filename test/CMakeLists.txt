cmake_minimum_required(VERSION 3.10)

set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/bulldozer)

find_package(GTest REQUIRED)

include_directories(${SRC_DIR})
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB SRC_FILES "${SRC_DIR}/*.cc")
file(GLOB TEST_FILES "${TEST_DIR}/*.cc")

add_executable(run_tests ${TEST_FILES} ${SRC_FILES})

target_link_libraries(run_tests
        PRIVATE
        GTest::GTest
        GTest::Main
        wolfssl)

include(GoogleTest)
gtest_discover_tests(run_tests)
enable_testing()



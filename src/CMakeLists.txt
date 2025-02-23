cmake_minimum_required(VERSION 3.10)

set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

find_package(CUDA REQUIRED)
find_package(gflags REQUIRED)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/bulldozer)

include_directories(${SRC_DIR})
include_directories(${CUDA_INCLUDE_DIRS})
enable_language(CUDA)

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cc)
file(GLOB_RECURSE CUDA_SRC_FILES ${SRC_DIR}/*.cu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(bulldozer bulldozer.cu ${SRC_FILES} ${CUDA_SRC_FILES})
target_link_libraries(bulldozer
        PRIVATE
        gflags
        pthread
        wolfssl
)

set_target_properties(bulldozer PROPERTIES
        CUDA_ARCHITECTURES 75
        CUDA_STANDARD 17
        CUDA_COMPILE_FEATURES cuda_std_17
        CUDA_SEPARABLE_COMPILATION ON
)
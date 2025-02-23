cmake_minimum_required(VERSION 3.10)

find_package(CUDA REQUIRED)
find_package(nvbench REQUIRED)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/bulldozer/cuda)
include_directories(${SRC_DIR})
include_directories(${CUDA_INCLUDE_DIRS})
enable_language(CUDA)

file(GLOB CUDA_SRC_FILES ${SRC_DIR}/*.cu)
message(STATUS "!!!!!CUDA_SRC_FILES: ${CUDA_SRC_FILES}")


add_executable(bulldozer_cuda_bench bulldozer_cuda_bench.cu ${CUDA_SRC_FILES})
target_link_libraries(bulldozer_cuda_bench
        PRIVATE
        ${CUDA_LIBRARIES}
        nvbench::nvbench
)

set_target_properties(bulldozer_cuda_bench PROPERTIES
    CUDA_ARCHITECTURES 75
    CUDA_STANDARD 17
    CUDA_COMPILE_FEATURES cuda_std_17
    CUDA_SEPARABLE_COMPILATION ON
)


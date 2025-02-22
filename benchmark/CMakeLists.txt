cmake_minimum_required(VERSION 3.24)

add_subdirectory(cpu)

find_package(CUDA REQUIRED)

if (CUDA_FOUND)
    add_subdirectory(cuda)
endif ()

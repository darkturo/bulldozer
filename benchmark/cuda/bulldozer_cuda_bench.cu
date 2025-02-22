#include <cuda_runtime.h>
#include <nvbench/nvbench.cuh>
#include <nvbench/main.cuh>
#include <sha256.cuh>
#include <string>
#include <vector>

 __global__ void sha256d_kernel(unsigned char *plaintext, size_t len, unsigned char *digest) {
    bulldozer::cuda::sha256d(plaintext, len, digest);
}

void sha256d_benchmark(nvbench::state& state) {
    const auto plaintext = state.get_string("message");
    unsigned char *d_plaintext = nullptr;
    cudaMalloc(&d_plaintext, plaintext.size());
    cudaMemcpy(d_plaintext, plaintext.data(), plaintext.size(), cudaMemcpyHostToDevice);

    unsigned char *d_digest = nullptr;
    cudaMalloc(&d_digest, 32);

    state.exec([=](nvbench::launch& launch) {
        sha256d_kernel<<<1, 1, 0, launch.get_stream()>>>(d_plaintext, plaintext.size(), d_digest);
    });

    cudaFree(d_plaintext);
    cudaFree(d_digest);
}

NVBENCH_BENCH(sha256d_benchmark)
        .set_name("sha256d_benchmark")
        .add_string_axis("message",
                         std::vector<std::string>{"abc",
                                                  "abcde",
                                                  "Lorem ipsum dolor sit amet, consectetur adipiscing duis.",
                                                  "Lorem ipsum dolor sit amet, consectetur adipiscing elit vivamus."});

NVBENCH_MAIN;
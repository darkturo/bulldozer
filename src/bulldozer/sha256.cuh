#pragma once

namespace bulldozer::cuda {
    constexpr uint32_t Sha256DigestSize = 32;

    __device__ void sha256(const unsigned char *data, size_t len, unsigned char *hash);

    __device__ void sha256d(const unsigned char *data, size_t len, unsigned char *hash);

}

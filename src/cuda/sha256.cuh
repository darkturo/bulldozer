#pragma once

namespace bulldozer::cuda {

    __device__ void cuhash_sha256(const unsigned char *data, size_t len, unsigned char *hash);

}

#pragma once

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>

#include <vector>

namespace bulldozer {
    std::vector <uint8_t> sha256_digest(const std::vector <uint8_t> &message) {
        byte hash[WC_SHA256_DIGEST_SIZE];

        wc_Sha256 sha256;

        wc_InitSha256(&sha256);
        wc_Sha256Update(&sha256, reinterpret_cast<const byte *>(message.data()), message.size());
        wc_Sha256Final(&sha256, hash);

        return std::vector<uint8_t>(hash, hash + WC_SHA256_DIGEST_SIZE);
    }

    std::vector <uint8_t> sha256d_digest(const std::vector <uint8_t> &message) {
        return sha256_digest(sha256_digest(message));
    }
}
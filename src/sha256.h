#pragma once

#include <span>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>

#include "constraints.h"


namespace bulldozer {
    const uint32_t DigestSize = WC_SHA256_DIGEST_SIZE;

    template <ByteType T>
    void sha256_digest(const std::span<T> message, std::span<T> digest) {
        // PRE: digest.size() == DigestSize
        wc_Sha256 sha256;

        wc_InitSha256(&sha256);
        wc_Sha256Update(&sha256, reinterpret_cast<const byte *>(message.data()), message.size());
        wc_Sha256Final(&sha256, reinterpret_cast<byte *>(digest.data()));
    }

    template <ByteType T>
    void sha256d_digest(const std::span<T> message, std::span<T> digest) {
        // PRE: digest.size() == DigestSize
        sha256_digest(message, digest);
        sha256_digest(digest, digest);
    }
}
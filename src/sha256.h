#pragma once

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>

#include <span>

namespace bulldozer {
    const uint32_t DigestSize = WC_SHA256_DIGEST_SIZE;

    template<typename T>
    concept ByteType = requires(T a) {
        { a } -> std::convertible_to<uint8_t>;
    };

    template <ByteType T>
    void sha256_digest(const std::span<T> message, std::span<T> digest) {
        // PRE: digest.size() == DigestSize
        wc_Sha256 sha256;

        wc_InitSha256(&sha256);
        wc_Sha256Update(&sha256, reinterpret_cast<const byte *>(message.data()), message.size());
        wc_Sha256Final(&sha256, reinterpret_cast<byte *>(digest.data()));
    }

    void sha256d_digest(const std::span<uint8_t> message, std::span<uint8_t> digest) {
        // PRE: digest.size() == DigestSize
        sha256_digest(message, digest);
        sha256_digest(digest, digest);
    }
}
#include <stdint.h>
#include <stddef.h>

#include "sha256.cuh"

namespace bulldozer::cuda {

    static __device__ uint32_t K[64] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    static __device__ __inline__ uint32_t rotr(uint32_t x, uint32_t n) {
        return (x >> n) | (x << (32 - n));
    }

    static __device__ __inline__ uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) ^ (~x & z);
    }

    static __device__ __inline__ uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    static __device__ __inline__ uint32_t sig0(uint32_t x) {
        return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
    }

    static __device__ __inline__ uint32_t sig1(uint32_t x) {
        return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
    }

    static __device__ __inline__ uint32_t theta0(uint32_t x) {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }

    static __device__ __inline__ uint32_t theta1(uint32_t x) {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }

    static __device__ __inline__ void
    sha256_pad(const unsigned char *data, size_t len, unsigned char *padded, size_t *padded_len) {
        size_t bit_len = len * 8;
        size_t num_blocks = (len + 9 + 63) / 64;
        *padded_len = num_blocks * 64;

        for (size_t i = 0; i < len; ++i) {
            padded[i] = data[i];
        }

        padded[len] = 0x80;

        for (size_t i = len + 1; i < *padded_len - 8; ++i) {
            padded[i] = 0x00;
        }

        for (int i = 0; i < 8; i++) {
            padded[*padded_len - 1 - i] = (bit_len >> (8 * i)) & 0xFF;
        }
    }

    __device__ __inline__ void sha256_compress(uint32_t *state, const unsigned char *block) {
        uint32_t w[64];
        uint32_t a, b, c, d, e, f, g, h;

#pragma unroll 16
        for (int i = 0; i < 16; i++) {
            w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | (block[i * 4 + 3]);
        }
#pragma unroll 48
        for (int i = 16; i < 64; i++) {
            w[i] = theta1(w[i - 2]) + w[i - 7] + theta0(w[i - 15]) + w[i - 16];
        }

        a = state[0];
        b = state[1];
        c = state[2];
        d = state[3];
        e = state[4];
        f = state[5];
        g = state[6];
        h = state[7];

#pragma unroll 64
        for (int i = 0; i < 64; i++) {
            uint32_t t1 = h + sig1(e) + ch(e, f, g) + K[i] + w[i];
            uint32_t t2 = sig0(a) + maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;
        state[5] += f;
        state[6] += g;
        state[7] += h;
    }

    __device__ void cuhash_sha256(const unsigned char *data, size_t len, unsigned char *hash) {
        // Initial hash values (first 32 bits of the fractional parts of the square roots of the first 8 primes)
        uint32_t h[8] = {
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };

        unsigned char padded_data[256];  // Adjust size based on expected input
        size_t padded_len;
        sha256_pad(data, len, padded_data, &padded_len);

        for (size_t i = 0; i < padded_len; i += 64) {
            sha256_compress(h, &padded_data[i]);
        }

#pragma unroll 8
        for (int i = 0; i < 8; i++) {
            hash[i * 4] = (h[i] >> 24) & 0xFF;
            hash[i * 4 + 1] = (h[i] >> 16) & 0xFF;
            hash[i * 4 + 2] = (h[i] >> 8) & 0xFF;
            hash[i * 4 + 3] = h[i] & 0xFF;
        }
    }

    __device__ void sha256d(const unsigned char *data, size_t len, unsigned char *hash) {
        unsigned char prehash[32];
        cuhash_sha256(data, len, prehash);
        cuhash_sha256(prehash, 32, hash);
    }

}
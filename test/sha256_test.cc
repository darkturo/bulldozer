// SHA256-d smoke tests

#include <gtest/gtest.h>
#include <sha256.h>

#include "test_util.h"


TEST(Sha256SmokeTest, basic) {
    std::vector<uint8_t> plaintext = {'a', 'b', 'c'};
    std::vector<uint8_t> expected = {
            0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
            0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };

    unsigned char buffer[bulldozer::DigestSize];
    std::span<unsigned char> hash(buffer);
    bulldozer::sha256_digest(std::span<unsigned char>(plaintext), hash);

    EXPECT_EQ(hash.size(), 32);
    EXPECT_EQUAL_SPAN(hash, expected);
}

TEST(Sha256SmokeTest, double_sha_basic) {
    std::vector<uint8_t> plaintext = {'a', 'b', 'c'};
    std::vector<uint8_t> expected = {
            0x4f, 0x8b, 0x42, 0xc2, 0x2d, 0xd3, 0x72, 0x9b, 0x51, 0x9b, 0xa6, 0xf6, 0x8d, 0x2d, 0xa7, 0xcc,
            0x5b, 0x2d, 0x60, 0x6d, 0x05, 0xda, 0xed, 0x5a, 0xd5, 0x12, 0x8c, 0xc0, 0x3e, 0x6c, 0x63, 0x58
    };

    unsigned char buffer[bulldozer::DigestSize];
    std::span<unsigned char> hash(buffer);
    bulldozer::sha256_digest(std::span<unsigned char>(plaintext), hash);
    bulldozer::sha256_digest(hash, hash);

    EXPECT_EQ(hash.size(), 32);
    EXPECT_EQUAL_SPAN(hash, expected);

    unsigned char buffer2[bulldozer::DigestSize];
    std::span<unsigned char> hash2(buffer);
    bulldozer::sha256d_digest(std::span<unsigned char>(plaintext), hash2);
    EXPECT_EQ(hash2.size(), 32);
    EXPECT_EQUAL_SPAN(hash2, expected);
}
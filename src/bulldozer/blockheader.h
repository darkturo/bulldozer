#pragma once

namespace bulldozer {
    namespace bitcoin {
        constexpr int32_t Version = 0x4;
        constexpr uint32_t DigestSize = 32;
        struct BlockHeader {
            int32_t version = Version;
            char[DigestSize] prev_block_header_digest;
            char[DigestSize] merkle_root_digest;
            uint32_t timestamp;
            uint32_t bits;
            uint32_t nonce = 0;
        };
    }
}
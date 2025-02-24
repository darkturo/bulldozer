#pragma once

#include <queue>
#include <mutex>
#include <string>
#include <thread>

#include "blockheader.h"
#include "merkle.h"
#include "sha256.h"

namespace bulldozer {
    constexpr uint32_t QueueSize = 10'000;
    class BlockHeaderGenerator {
    public:
        BlockHeaderGenerator(::MerkleTree<char> &merkle_tree,
                             uint32_t block_height,
                             std::string miner_tag,
                             std::string pay_to_btc_address,
                             char[::bitcoin::DigestSize] prev_block_hash,
                             uint32_t seconds_from_now = 0
                             );
        startGenerator();
        stopGenerator();

        bool isEmpty();j
        ::bitcoin::BlockHeader getNextBlockHeader(); // Blocking call
    private:
        bool m_is_active;
        std::queue<::bitcoin::BlockHeader> m_queue;
        std::mutex m_mutex;
        std::thread th_generator;

        ::MerkleTree<char> &m_merkle_tree;
        uint32_t m_block_height;
        std::string m_miner_tag;
        std::string m_pay_to_btc_address;
        char m_prev_block_hash[::bitcoin::DigestSize];
        uint32_t m_seconds_from_now;
    };
}

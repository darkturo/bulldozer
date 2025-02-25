#include "blockheader_generator.h"

#include <condition_variable>
#include <mutex>
#include "sha256.h"


namespace bulldozer {
    BlockHeaderGenerator::BlockHeaderGenerator(::MerkleTree<char> &merkle_tree,
                                               std::string miner_tag,
                                               std::string pay_to_btc_address,
                                               char[::bitcoin::DigestSize] prev_block_hash,
                                               uint32_t block_height,
                                               uint32_t block_time,
                                               uint32_t difficulty_target
                                               ):
        m_is_active(false),
        m_merkle_tree(merkle_tree),
        m_block_height(block_height),
        m_miner_tag(miner_tag),
        m_pay_to_btc_address(pay_to_btc_address),
        m_prev_block_hash(prev_block_hash),
        m_block_time(block_time)
    {
        m_queue.reserve(MaxQueueSize);
    }

    void BlockHeaderGenerator::startGenerator() {
        m_is_active = true;

        th_generator = std::thread(&BlockHeaderGenerator::blockHeaderGenerator, this);
    }

    void BlockHeaderGenerator::stopGenerator() {
        m_is_active = false;
        m_cv.notify_all();
        th_generator.join();
    }

    bool BlockHeaderGenerator::isEmpty() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    ::bitcoin::BlockHeader BlockHeaderGenerator::getNextBlockHeader() {
        ::bitcoin::BlockHeader blockHeader

        {
            std::unique_lock <std::mutex> lock(m_mutex);
            if (m_queue.empty()) {
                m_cv.wait(lock, m_is_active && !m_queue.empty(); );
            }
            blockHeader = m_queue.front();
            m_queue.pop();
        }
        m_cv.notify_all();

        return blockHeader;
    }

    void BlockHeaderGenerator::blockHeaderGenerator() {
        while (m_is_active) {
            ::bitcoin::BlockHeader blockHeader = {
                .version = ::bitcoin::BlockVersion,
                .timestamp = m_block_time,
                .bits = m_difficulty_target,
                .nonce = 0U
            };
            std::copy(m_prev_block_hash, m_prev_block_hash + ::bitcoin::DigestSize,
                      blockHeader.prev_block_header_digest);

            // 1.generate coinbase tx


            // 2. make sha256 hash of coinbase tx
            // 3. update coinbase in the Merkletree
            // 4. calculate merkle root MerkleTree::calculate_merkle_root()
            // 5. update blockHeader.merkle_root_digest


            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_queue.size() == MaxQueueSize) {
                    m_cv.wait(lock, m_is_active && (m_queue.size() < MinQueueSize));
                }
                m_queue.push(bh);
            }
            m_cv.notify_all();
        }
    }

}
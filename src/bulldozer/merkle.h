#pragma once

#include <algorithm>
#include <cassert>
#include <span>

#include "constraints.h"


namespace bulldozer {
    template <ByteType T>
    class MerkleTree {
    public:
        MerkleTree(size_t num_leaves);
        ~MerkleTree();
        void add_txid(std::span<T> txid);
        void update_coinbase(std::span<T> coinbase);
        void calculate_merkle_root(std::span<T> output);

    private:
        size_t m_numLeaves;
        size_t m_index;
        T *p_txids;
    };

    template <ByteType T>
    void RPC_to_internal(std::span<T> input, std::span<T> output) {
        assert(input.size() == output.size());
        std::reverse_copy(input.begin(), input.end(), output.begin());
    }

    template <ByteType T>
    void internal_to_RPC(std::span<T> input, std::span<T> output) {
        assert(input.size() == output.size());
        std::reverse_copy(input.begin(), input.end(), output.begin());
    }
}
#include "merkle.h"
#include <algorithm>
#include <cassert>
#include <span>
#include <vector>

#include "constraints.h"
#include "sha256.h"


namespace bulldozer {
    template <ByteType T>
    void concatenate(std::span<T> first, std::span<T> second, std::span<T> output) {
        assert(first.size() == second.size() && first.size() + second.size() == output.size());
        std::copy(first.begin(), first.end(), output.begin());
        std::copy(second.begin(), second.end(), output.begin() + first.size());
    }

    template <ByteType T>
    MerkleTree<T>::MerkleTree(size_t num_leaves) : m_numLeaves(num_leaves), m_index(0) {
        // num_leaves includes the coinbase transaction and the other transactions
        p_txids = new T[num_leaves * DigestSize];
    }

    template <ByteType T>
    MerkleTree<T>::~MerkleTree() {
        delete[] p_txids;
    }

    template <ByteType T>
    void MerkleTree<T>::add_txid(std::span<T> txid) {
        assert(m_index < m_numLeaves);
        std::span<T> output(p_txids + m_index * DigestSize, DigestSize);
        std::copy(txid.begin(), txid.end(), output.begin());
        m_index++;
    }

    template <ByteType T>
    void MerkleTree<T>::update_coinbase(std::span<T> coinbase) {
        std::span<T> coinbase_txid(p_txids, DigestSize);
        sha256d_digest<T>(coinbase, coinbase_txid);
    }

    template <ByteType T>
    void MerkleTree<T>::calculate_merkle_root(std::span<T> output) {
        assert(output.size() == DigestSize);

        if (m_numLeaves == 1) {
            std::span<T> root(p_txids, DigestSize);
            std::copy(root.begin(), root.end(), output.begin());
            return;
        }

        // prepare the input
        std::vector<std::vector<T>> merkle_leaves;
        merkle_leaves.reserve(m_numLeaves);
        for (size_t i = 0; i < m_numLeaves; i++) {
            std::span<T> leave(p_txids + i * DigestSize, DigestSize);
            merkle_leaves.push_back(std::vector<T>(leave.begin(), leave.end()));
        }

        while (merkle_leaves.size() > 1) {
            // if number of hashes is odd, duplicate the last hash in the list
            if (merkle_leaves.size() % 2 != 0) {
                merkle_leaves.push_back(merkle_leaves.back());
            }
            assert(merkle_leaves.size() % 2 == 0); // merkle size should be even at this point

            std::vector<std::vector<T>> merkle_nodes;
            merkle_nodes.reserve(merkle_leaves.size() / 2);

            for (auto it = merkle_leaves.begin(); it != merkle_leaves.end(); it+=2) {
                T buffer[DigestSize * 2];
                concatenate<T>(*it, *(it + 1), std::span<T>(buffer));

                std::vector<T> hash(DigestSize);
                sha256d_digest<T>(std::span<T>(buffer), std::span<T>(hash));

                merkle_nodes.push_back(hash);
            }

            merkle_leaves = merkle_nodes;
        }

        auto root = merkle_leaves.front();
        std::copy(root.begin(), root.end(), output.begin());
    }
}

template class bulldozer::MerkleTree<uint8_t>;
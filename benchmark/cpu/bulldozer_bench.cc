#include <folly/Benchmark.h>
#include <gflags/gflags.h>
#include <string>
#include <vector>
#include <random>

#include <sha256.h>
#include <merkle.h>


BENCHMARK(sha256_digest, n) {
    std::string message = "abc";
    std::vector<char> digest(bulldozer::DigestSize);

    for (int i = 0 ; i < n; i++) {
        bulldozer::sha256_digest(std::span<char>(message),
                                 std::span<char>(digest));

        BENCHMARK_SUSPEND {
            folly::doNotOptimizeAway(digest);
        }
    }
}

BENCHMARK(double_sha256_digest, n) {
    std::string message = "abc";
    std::vector<char> digest(bulldozer::DigestSize);

    for (int i = 0 ; i < n; i++) {
        bulldozer::sha256d_digest(std::span<char>(message),
                                  std::span<char>(digest));

        BENCHMARK_SUSPEND {
            folly::doNotOptimizeAway(digest);
        }
    }
}

void merkle_tree_initialization(int iter, int txs) {
    auto gen = std::mt19937(42);
    auto dist = std::uniform_int_distribution<uint64_t>(0, 2^64);

    for (int k = 0; k < iter; k++) {
        bulldozer::MerkleTree<uint8_t> tree(txs);

        for (int i = 0; i < txs; i++) {
            uint8_t buf[32];
            std::vector<uint8_t> digest(bulldozer::DigestSize);
            BENCHMARK_SUSPEND{
                    for (int j = 0; j < 4; j++) {
                        auto d = dist(gen);
                        std::copy(reinterpret_cast<uint8_t *>(&d),
                                  reinterpret_cast<uint8_t *>(&d) + sizeof(d),
                                  buf + j * sizeof(d));
                    }
                    bulldozer::sha256d_digest(std::span<uint8_t>(buf),
                                              std::span<uint8_t>(digest));
            };
            tree.add_txid(std::span<uint8_t>(digest));
        }
    }
}
BENCHMARK_NAMED_PARAM(merkle_tree_initialization, 10_txs, 10);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_initialization, 100_txs, 100);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_initialization, 700_txs, 100);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_initialization, 1000_txs, 1000);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_initialization, 2500_txs, 1000);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_initialization, 3500_txs, 1000);


void merkle_tree_calculate(int iter, int txs) {
    bulldozer::MerkleTree<uint8_t> tree(txs);
    BENCHMARK_SUSPEND {
        auto gen = std::mt19937(42);
        auto dist = std::uniform_int_distribution<uint64_t>(0, 2^64);
        for (int i = 0; i < txs; i++) {
            uint8_t buf[32];
            for (int j = 0; j < 4; j++) {
                auto d = dist(gen);
                std::copy(reinterpret_cast<uint8_t*>(&d),
                          reinterpret_cast<uint8_t*>(&d) + sizeof(d),
                          buf + j * sizeof(d));
            }
            std::vector<uint8_t> digest(bulldozer::DigestSize);
            bulldozer::sha256d_digest(std::span<uint8_t>(buf),
                                      std::span<uint8_t>(digest));
            tree.add_txid(std::span<uint8_t>(digest));
        }
    }

    for (int i = 0; i < iter; i++) {
        std::vector<uint8_t> root(bulldozer::DigestSize);
        tree.calculate_merkle_root(std::span<uint8_t>(root));
        BENCHMARK_SUSPEND {
                folly::doNotOptimizeAway(root);
        }
    }
}
BENCHMARK_NAMED_PARAM(merkle_tree_calculate, 10_txs, 10);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_calculate, 100_txs, 100);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_calculate, 700_txs, 100);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_calculate, 1000_txs, 1000);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_calculate, 2500_txs, 1000);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_calculate, 3500_txs, 1000);

void merkle_tree_recalculation(int iter, int recalc_tiems) {
    bulldozer::MerkleTree<uint8_t> tree(2500);
    auto gen = std::mt19937(42);
    auto dist = std::uniform_int_distribution<uint64_t>(0, 2^64);
    BENCHMARK_SUSPEND {
            for (int i = 0; i < 2500; i++) {
                uint8_t buf[32];
                for (int j = 0; j < 4; j++) {
                    auto d = dist(gen);
                    std::copy(reinterpret_cast<uint8_t*>(&d),
                              reinterpret_cast<uint8_t*>(&d) + sizeof(d),
                              buf + j * sizeof(d));
                }
                std::vector<uint8_t> digest(bulldozer::DigestSize);
                bulldozer::sha256d_digest(std::span<uint8_t>(buf),
                                          std::span<uint8_t>(digest));
                tree.add_txid(std::span<uint8_t>(digest));
            }
            std::vector<uint8_t> root(bulldozer::DigestSize);
            tree.calculate_merkle_root(std::span<uint8_t>(root));
    }

    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < recalc_tiems; j++) {
            std::vector<uint8_t> digest(bulldozer::DigestSize);
            BENCHMARK_SUSPEND {
                    uint8_t buf[32];
                    for (int j = 0; j < 4; j++) {
                        auto d = dist(gen);
                        std::copy(reinterpret_cast<uint8_t*>(&d),
                                  reinterpret_cast<uint8_t*>(&d) + sizeof(d),
                                  buf + j * sizeof(d));
                    }
                    bulldozer::sha256d_digest(std::span<uint8_t>(buf),
                                              std::span<uint8_t>(digest));
            }
            tree.update_coinbase(std::span<uint8_t>(digest));
            std::vector<uint8_t> root(bulldozer::DigestSize);
            tree.calculate_merkle_root(std::span<uint8_t>(root));
        }
    }
}
BENCHMARK_NAMED_PARAM(merkle_tree_recalculation, 1000_reps, 1000);
BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_recalculation, 10000_reps, 1000);
//BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_recalculation, 100000_reps, 100000);
//BENCHMARK_RELATIVE_NAMED_PARAM(merkle_tree_recalculation, 1000000_reps, 1000000);


int main(int argc, char **argv) {
    gflags::SetUsageMessage("WolfSSL SHA256 benchmark");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    folly::runBenchmarks();
    return 0;
}
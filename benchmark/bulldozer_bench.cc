#include <folly/Benchmark.h>
#include <gflags/gflags.h>
#include <string>
#include <vector>

#include <sha256.h>


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

int main(int argc, char **argv) {
    gflags::SetUsageMessage("WolfSSL SHA256 benchmark");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    folly::runBenchmarks();
    return 0;
}
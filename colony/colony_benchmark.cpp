#include "benchmark/benchmark.h"

#include "colony.h"

static void BM_FirstBench(benchmark::State& state) {
  for (auto _ : state) {
    int sum{};
    std::vector<int> vec;
    vec.resize(100000);
    for (auto i : vec) {
      sum += i;
    }
  }
}
// Register the function as a benchmark
BENCHMARK(BM_FirstBench);

BENCHMARK_MAIN();
#include "benchmark/benchmark.h"

#include "neural.h"

static void BM_FirstBench(benchmark::State& state) {
  for (auto _ : state) {
  }
}
// Register the function as a benchmark
BENCHMARK(BM_FirstBench);

BENCHMARK_MAIN();
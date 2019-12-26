#include "benchmark/benchmark.h"

#include "neural2.h"

static void BM_FirstBench(benchmark::State& state) {
  Network<Sigmoid> network;
  network.SetLayout({2, 100, 100, 100, 100, 1});
  network.RandomizeWeights();
  network.SetLearnFactor(0.9);

  for (auto _ : state) {
    network.SetInput({0.0, 0.0});
    network.FeedForward();
    network.BackPropagate({0.0});
  }
}
// Register the function as a benchmark
BENCHMARK(BM_FirstBench);

BENCHMARK_MAIN();
#include <chrono>
#include <iomanip>
#include <iostream>
#include "neural2.h"

using namespace std;

void PrintWeights(const std::vector<Weights>& weights) {
  for (const auto& weight : weights) {
    cout << "Weights:" << endl;
    for (size_t row{}; row < weight.Ninput(); ++row) {
      for (size_t column{}; column < weight.Noutput(); ++column) {
        cout << weight(row, column) << " ";
      }
      cout << endl;
    }
  }
}

template <typename TF>
double PrintErrors(Network<TF>& network) {
  network.SetInput({0.0, 0.0});
  network.FeedForward();
  const double error_1 = network.GetError({0.0});
  network.SetInput({0.0, 1.0});
  network.FeedForward();
  const double error_2 = network.GetError({1.0});
  network.SetInput({1.0, 0.0});
  network.FeedForward();
  const double error_3 = network.GetError({1.0});
  network.SetInput({1.0, 1.0});
  network.FeedForward();
  const double error_4 = network.GetError({0.0});

  const double error_total = error_1 + error_2 + error_3 + error_4;

  cout << "Case1 error: " << error_1 << endl;
  cout << "Case2 error: " << error_2 << endl;
  cout << "Case3 error: " << error_3 << endl;
  cout << "Case4 error: " << error_4 << endl;
  cout << "Total error: " << error_total << endl;
  cout << "------------------------------------------" << endl;

  return error_total;
}

int main() {
  Network<Tanh> network;
  network.SetLayout({2, 100, 100, 1}, false);
  network.RandomizeWeights();
  network.SetLearnFactor(1.0 / (100));

  // PrintWeights(network.GetWeights());

  double error = PrintErrors(network);
  int n{};

  auto start = std::chrono::high_resolution_clock::now();

  // while (error > 0.001) {
  while (n < 10'000) {
    network.SetInput({0.0, 0.0});
    network.FeedForward();
    const auto w1 = network.BackPropagate({0.0});

    network.SetInput({0.0, 1.0});
    network.FeedForward();
    const auto w2 = network.BackPropagate({1.0});

    network.SetInput({1.0, 0.0});
    network.FeedForward();
    const auto w3 = network.BackPropagate({1.0});

    network.SetInput({1.0, 1.0});
    network.FeedForward();
    const auto w4 = network.BackPropagate({0.0});

    network.AddWeights(w1);
    network.AddWeights(w2);
    network.AddWeights(w3);
    network.AddWeights(w4);

    if (++n % 1000 == 0) {
      cout << "Run: " << n << endl;
      error = PrintErrors(network);
    }
  }

  auto finish = std::chrono::high_resolution_clock::now();

  cout << "Run: " << n << endl;
  PrintErrors(network);

  std::chrono::duration<double> elapsed = finish - start;

  std::cout << endl << "Elapsed time: " << elapsed.count() << " s" << endl;

  // PrintWeights(network.GetWeights());

  return 0;
}
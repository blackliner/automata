#include <iomanip>
#include <iostream>
#include "neural.h"

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

double PrintErrors(Network& network) {
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
  Network network;
  network.SetLayout({2, 5, 4, 3, 1});
  network.RandomizeWeights();
  network.SetTFtype(TFtype::SIGMOID);
  network.SetLearnFactor(0.9);
  PrintWeights(network.GetWeights());

  double error = PrintErrors(network);

  while (error > 0.1) {
    network.SetInput({0.0, 0.0});
    network.FeedForward();
    network.BackPropagate({0.0});

    network.SetInput({0.0, 1.0});
    network.FeedForward();
    network.BackPropagate({1.0});

    network.SetInput({1.0, 0.0});
    network.FeedForward();
    network.BackPropagate({1.0});

    network.SetInput({1.0, 1.0});
    network.FeedForward();
    network.BackPropagate({0.0});

    error = PrintErrors(network);
  }

  PrintWeights(network.GetWeights());

  return 0;
}
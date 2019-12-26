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

int main() {
  Network network;
  network.SetLayout({2, 5, 4, 3, 1});
  network.ResetWeights(1.0);
  network.SetInput({0.5, 1.0});
  network.SetTFtype(TFtype::SIGMOID);
  network.SetLearnFactor(0.1);
  PrintWeights(network.GetWeights());

  Data target{0.0};

  network.FeedForward();
  double error = network.GetError(target);
  cout << "Initial error: " << error << endl;

  while (error > 0.15) {
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

    error = network.GetError(target);
    cout << "Current error: " << error << endl;
  }

  PrintWeights(network.GetWeights());

  return 0;
}
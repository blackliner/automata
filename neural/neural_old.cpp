// NeuralNetwork_MK1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
//#include <cstdlib>
#include <cassert>
#include <cmath>

using namespace std;

class Neuron;
typedef vector<Neuron> Layer;

//***************************** Neuron Class ****************************

class Neuron {
 public:
  Neuron(size_t numOfInputs, int myIndex);

  void setOutputValue(const double val) {
    m_outVal = val;
  }
  double getOutputValue(void) const {
    return m_outVal;
  }
  void feedForward(const Layer& prevLayer);
  void calcOutputGradients(double targetVal);
  void calcHiddenGradients(const Layer& nextLayer);
  void calcInputWeights(Layer& prevLayer);
  void publishInputWeights(int numberOfBackPropIterations);

 private:
  static double eta;
  static double alpha;
  static double transferFunction(double x);
  static double transferFunctionDerivative(double x);
  static double randomWeight(void) {
    double random_value = static_cast<double>(rand());
    double random_max{RAND_MAX};
    double return_value = 2 * random_value / random_max;
    return return_value - 1;
    // old  only gave negative values... return 2 * rand() / double(RAND_MAX) - 1;
  };
  double sumDOW(const Layer& nextLayer) const;
  double m_outVal;
  int m_myIndex;
  vector<double> m_inWeight;
  vector<double> m_deltaWeight;
  double m_gradient;

  vector<double> m_deltaWeightStorage;
  // vector<double> m_deltaWeightStorage;

  // flo speciale
  // Layer m_prevLayer; // GEIL DER & HATS GEBRACHT, vorher hat er immer kopien gemacht!!!! Ok & geht auch nicht, ich
  // probiers mit pointern
};

void Neuron::publishInputWeights(int numberOfBackPropIterations) {
  for (size_t n{}; n < m_inWeight.size(); n++) {
    m_deltaWeight[n] = m_deltaWeightStorage[n] / (double)numberOfBackPropIterations;
    m_inWeight[n] += m_deltaWeight[n];

    m_deltaWeightStorage[n] = 0.0;
  }
}

double Neuron::eta = 0.15;
double Neuron::alpha = 0.5;

void Neuron::calcInputWeights(Layer& prevLayer) {
  for (size_t n{}; n < prevLayer.size(); n++) {
    Neuron& neuron = prevLayer[n];

    double oldDeltaWeight = m_deltaWeight[n];

    double newDeltaWeight =
        // individual input, magnified by the gradient and train rate
        eta  // 0.0 slow, 0.2 medium, 1.0 reckless learner
            * neuron.getOutputValue() * m_gradient
        // also add momentum
        + alpha  // 0.0 no Momentum, 0.5 moderate
              * oldDeltaWeight;

    // hier gehört noch das deltaWeight mitgerechnet: m_inweight[n].delta
    // m_deltaWeight[n] = newDeltaWeight;
    // m_inWeight[n] += newDeltaWeight;

    // m_deltaWeightStorage[n] = newDeltaWeight;
    m_deltaWeightStorage[n] += newDeltaWeight;
  }
}

double Neuron::sumDOW(const Layer& nextLayer) const {
  double sum = 0.0;

  // sum our contr. of the errors at the nodes we feed

  for (size_t n{}; n < nextLayer.size() - 1; n++) {
    sum += nextLayer[n].m_inWeight[m_myIndex] * nextLayer[n].m_gradient;
  }

  return sum;
}

void Neuron::calcHiddenGradients(const Layer& nextLayer) {
  double dow = sumDOW(nextLayer);
  m_gradient = dow * Neuron::transferFunctionDerivative(m_outVal);
}

void Neuron::calcOutputGradients(double targetVal) {
  double delta = targetVal - m_outVal;
  m_gradient = delta * Neuron::transferFunctionDerivative(m_outVal);
}

double Neuron::transferFunction(double x) {
  // tanh - outrange -1 ... 1
  return tanh(x);
}

double Neuron::transferFunctionDerivative(double x) {
  // tanh derivative approximation
  return 1.0 - x * x;
}

Neuron::Neuron(size_t numOfInputs, int myIndex) {
  for (size_t i{}; i < numOfInputs; i++) {
    m_inWeight.push_back(randomWeight());

    m_deltaWeight.push_back(0.0);
    m_deltaWeightStorage.push_back(0.0);
  }

  m_myIndex = myIndex;
}

void Neuron::feedForward(const Layer& prevLayer) {
  double sum = 0.0;

  // sum up all prev outputs
  for (size_t i{}; i < prevLayer.size(); i++) {
    sum += prevLayer[i].m_outVal * m_inWeight[i];
  }

  m_outVal = transferFunction(sum);
}

//***************************** Net Class ****************************

class Net {
 public:
  Net(const vector<unsigned>& topology);
  void feedForward(const vector<double>& inputVals);
  void backProp(const vector<double>& targetVals);
  void publishBackProp(void);
  void getResults(vector<double>& resultVals) const;
  double getRecentAvgError(void) {
    return m_recentAvgError;
  };

 private:
  vector<Layer> m_layers;
  double m_error;
  double m_recentAvgError = 100;
  static double m_recentAvgErrorSmth;
  int numberOfBackPropIterations = 0;
};

double Net::m_recentAvgErrorSmth = 10;

void Net::getResults(vector<double>& resultVals) const {
  resultVals.clear();

  for (size_t n{}; n < m_layers.back().size() - 1; n++) {
    resultVals.push_back(m_layers.back()[n].getOutputValue());
  }
}

void Net::publishBackProp() {
  if (numberOfBackPropIterations == 0) return;

  for (size_t layerNum = m_layers.size() - 1; layerNum > 0; layerNum--) {
    Layer& layer = m_layers[layerNum];

    for (size_t n{}; n < layer.size() - 1; n++) {
      layer[n].publishInputWeights(numberOfBackPropIterations);
    }
  }

  numberOfBackPropIterations = 0;
}

void Net::backProp(const vector<double>& targetVals) {
  // calculate overall net error (RMS)

  Layer& outputLayer = m_layers.back();
  m_error = 0.0;

  for (size_t n{}; n < outputLayer.size() - 1; n++) {
    double delta = targetVals[n] - outputLayer[n].getOutputValue();

    m_error += delta * delta;
  }

  m_error /= outputLayer.size() - 1;
  m_error = sqrt(m_error);

  // average error
  m_recentAvgError = (m_recentAvgError * m_recentAvgErrorSmth + m_error) / (m_recentAvgErrorSmth + 1);

  // calculate output layer gradients

  for (size_t n{}; n < outputLayer.size() - 1; n++) {
    outputLayer[n].calcOutputGradients(targetVals[n]);
  }

  // calculate gradient on hidden layers

  for (size_t layerNum = m_layers.size() - 2; layerNum > 0; layerNum--) {
    Layer& hiddenLayer = m_layers[layerNum];
    Layer& nextLayer = m_layers[layerNum + 1];

    for (size_t n{}; n < hiddenLayer.size(); n++) {
      hiddenLayer[n].calcHiddenGradients(nextLayer);
    }
  }

  // uppdate weights from output to first hidden layer

  for (size_t layerNum = m_layers.size() - 1; layerNum > 0; layerNum--) {
    Layer& layer = m_layers[layerNum];
    Layer& prevLayer = m_layers[layerNum - 1];

    for (size_t n{}; n < layer.size() - 1; n++) {
      layer[n].calcInputWeights(prevLayer);
    }
  }

  numberOfBackPropIterations++;
}

Net::Net(const vector<unsigned>& topology) {
  int numOfInputs = 0;

  for (auto layerNum : topology) {
    m_layers.push_back(Layer());

    // not needed at all, bec 1st layer directly gets the outval from networks input
    //// hack for getting 1 weight for every input Neuron !?
    // temp.push_back(Neuron(temp));

    for (unsigned i = 0; i < layerNum; i++) {
      // first Layer is inputLayer, which has no prevLayer :-(

      m_layers.back().push_back(Neuron(numOfInputs, i));

      cout << "adding Neuron" << endl;
    }

    // add on-neuron as bias thingy
    m_layers.back().push_back(Neuron(numOfInputs, layerNum));
    // define the always on thingy as an output of 1
    m_layers.back().back().setOutputValue(1.0);

    numOfInputs = m_layers.back().size();

    cout << "adding on-Neuron" << endl;
  }
}

void Net::feedForward(const vector<double>& inputVals) {
  assert(inputVals.size() == m_layers[0].size() - 1);

  for (size_t i{}; i < inputVals.size(); i++) {
    m_layers[0][i].setOutputValue(inputVals[i]);
  }

  // forward propagation
  for (size_t layerNum{1}; layerNum < m_layers.size(); layerNum++) {
    Layer& prevLayer = m_layers[layerNum - 1];

    for (size_t nodeNum{}; nodeNum < m_layers[layerNum].size() - 1; nodeNum++) {
      m_layers[layerNum][nodeNum].feedForward(prevLayer);
    }
  }
}

int main() {
  vector<unsigned> topology;
  topology.push_back(1000);
  topology.push_back(1000);
  topology.push_back(1000);
  // topology.push_back(48);
  // topology.push_back(48);
  topology.push_back(1);
  Net myNet(topology);

  vector<double> inputVals;
  vector<double> targetVals;
  vector<double> resultVals;
  bool in1, in2, in3, in4, out1;

  int runN = 0;
  double error = 1;

  while (error > 0.01) {
    // in1 = (rand() / double(RAND_MAX)) > 0.5;
    // in2 = (rand() / double(RAND_MAX)) > 0.5;
    // in3 = (rand() / double(RAND_MAX)) > 0.5;
    // in4 = (rand() / double(RAND_MAX)) > 0.5;
    // in1 = true;
    // in2 = false;
    // in3 = true;
    // in4 = false;
    in1 = (runN % 2) >= 1;
    in2 = (runN % 4) >= 2;
    in3 = (runN % 8) >= 4;
    in4 = (runN % 16) >= 8;

    // out1 = in1 ^ in2;
    out1 = in1 ^ in2 ^ in3 ^ in4;

    inputVals.clear();
    inputVals.push_back(in1);
    inputVals.push_back(in2);
    inputVals.push_back(in3);
    inputVals.push_back(in4);
    myNet.feedForward(inputVals);

    myNet.getResults(resultVals);

    // cout << "Run Number " << runN++ << endl;
    // cout << "in1 " << in1 << " in2 " << in2 << endl;
    // cout << "result " << resultVals.back() << endl;

    targetVals.clear();
    targetVals.push_back(out1);
    myNet.backProp(targetVals);

    if (runN % 16 == 0) {
      myNet.publishBackProp();

      myNet.feedForward(inputVals);
      myNet.getResults(resultVals);

      // cout << "after backProp result " << resultVals.back() << endl;
      // cout << "error " << myNet.getRecentAvgError() << endl << endl;

      error = myNet.getRecentAvgError();
      cout << error << endl;
    }

    runN++;
  }

  cout << "Run Number " << runN << endl;
  cout << "in1 " << in1 << " in2 " << in2 << endl;
  cout << "result " << resultVals.back() << endl;
  cout << "error " << myNet.getRecentAvgError() << endl << endl;

  return system("pause");
}

#include "rps.h"
#include <iostream>
#include <random>

using namespace std;

// Random number in [0,1)
static double random01() {
    static mt19937 rng(random_device{}());
    static uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

RPSTrainer::RPSTrainer()
    : regretSum(NUM_ACTIONS, 0.0),
      strategy(NUM_ACTIONS, 0.0),
      strategySum(NUM_ACTIONS, 0.0),
      opponentStrategy(NUM_ACTIONS, 0.0),
      oppRegretSum(NUM_ACTIONS, 0.0),
      oppStrategySum(NUM_ACTIONS, 0.0) {}


void RPSTrainer::train(int iterations) {
    for (int i = 0; i < iterations; i++) {
        getStrategy();

        int myAction = getAction(strategy);
        int oppAction = getAction(opponentStrategy);

        vector<double> myUtility = getActionUtilities(oppAction);
        vector<double> oppUtility = getActionUtilities(myAction);

        for (int a = 0; a < NUM_ACTIONS; a++) {
            regretSum[a] += myUtility[a] - myUtility[myAction];
            oppRegretSum[a] += oppUtility[a] - oppUtility[oppAction];
        }
    }

    cout << "Average strategy: ";
    for (int i = 0; i < NUM_ACTIONS; i++) {
        double avg = strategySum[i] / iterations;
        cout << avg << " ";
    }
    cout << endl;
}


void RPSTrainer::getStrategy() {
    double normalizingSum = 0.0;
    double opponentNormalizingSum = 0.0;

    for (int i = 0; i < NUM_ACTIONS; i++) {
        strategy[i] = regretSum[i] > 0 ? regretSum[i] : 0.0;
        opponentStrategy[i] = oppRegretSum[i] > 0 ? oppRegretSum[i] : 0.0;
        opponentNormalizingSum += opponentStrategy[i];
        normalizingSum += strategy[i];
    }

    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (normalizingSum > 0)
            strategy[i] /= normalizingSum;
        else
            strategy[i] = 1.0 / NUM_ACTIONS;

        if (opponentNormalizingSum > 0)
            opponentStrategy[i] /= opponentNormalizingSum;
        else
            opponentStrategy[i] = 1.0 / NUM_ACTIONS;

        oppStrategySum[i] += opponentStrategy[i];
        strategySum[i] += strategy[i];
    }
}

int RPSTrainer::getAction(vector<double> strat) {
    double r = random01();
    double cumulative = 0.0;

    for (int i = 0; i < NUM_ACTIONS; i++) {
        cumulative += strat[i];
        if (r < cumulative)
            return i;
    }
    return NUM_ACTIONS - 1;
}

vector<double> RPSTrainer::getActionUtilities(int opponentAction) {
    vector<double> utility(NUM_ACTIONS, 0.0);

    utility[(opponentAction + 1) % NUM_ACTIONS] = 1.0;
    utility[(opponentAction + 2) % NUM_ACTIONS] = -1.0;

    return utility;
}
